/***************************************************************************//**

  @file         log.c

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Logging facilities implementation for libstephen.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include "libstephen/cb.h"
#include "libstephen/log.h"

static smb_logger default_logger = {
  .format = SMB_DEFAULT_LOGFORMAT,
  .num = 0
};

static smb_logger *pdefault_logger;

static char *level_names[] = {"NOTSET", "DEBUG", "INFO",
                              "WARNING", "ERROR", "CRITICAL"};

static void reference_logger(smb_logger **obj)
{
  smb_status status = SMB_SUCCESS;
  if (*obj == NULL) {
    if (pdefault_logger == NULL) {
      sl_add_handler(&default_logger,
                     (smb_loghandler){.level = SMB_DEFAULT_LOGLEVEL,
                                      .dst   = SMB_DEFAULT_LOGDEST},
                     &status);
      pdefault_logger = &default_logger;
    }
    *obj = pdefault_logger;
  }
}

void sl_init(smb_logger *obj)
{
  obj->format = SMB_DEFAULT_LOGFORMAT;
  obj->num = 0;
}

smb_logger *sl_create(void)
{
  smb_logger *obj = smb_new(smb_logger, 1);
  sl_init(obj);
  return obj;
}

void sl_destroy(smb_logger *obj) {} // nothing to delete

void sl_delete(smb_logger *obj) {
  sl_destroy(obj);
  smb_free(obj);
}

void sl_set_level(smb_logger *obj, int level)
{
  int i;
  reference_logger(&obj);
  for (i = 0; i < obj->num; i++) {
    obj->handlers[i].level = level;
  }
}

void sl_add_handler(smb_logger *obj, smb_loghandler h, smb_status *status)
{
  reference_logger(&obj);
  if (obj->num < SMB_MAX_LOGHANDLERS) {
    obj->handlers[obj->num++] = h;
  } else {
    *status = SMB_INDEX_ERROR;
  }
}

void sl_clear_handlers(smb_logger *obj)
{
  reference_logger(&obj);
  obj->num = 0;
}

void sl_set_default_logger(smb_logger *obj)
{
  if (obj == NULL)
    obj = &default_logger;
  pdefault_logger = obj;
}

static char *sl_level_string(int level) {
  static char buf[20]; // plenty of space, to be safe.
  if (level % 10 == 0 && level >= LNOTSET && level <= LCRITICAL) {
    return level_names[level / 10];
  } else {
    snprintf(buf, 20, "%d", level);
    return buf;
  }
}

void sl_log(smb_logger *obj, char *file, int line, const char *function, int level, ...) {
  cbuf file_line_buf, message_buf;
  char *level_string, *format;
  va_list va;
  int i;

  reference_logger(&obj);

  cb_init(&file_line_buf, 256);
  cb_printf(&file_line_buf, "%s:%d", file, line);

  va_start(va, level);
  format = va_arg(va, char*);
  cb_init(&message_buf, 1024);
  cb_vprintf(&message_buf, format, va);
  va_end(va);

  level_string = sl_level_string(level);
  for (i = 0; i < obj->num; i++) {
    if (obj->handlers[i].level <= level) {
      fprintf(obj->handlers[i].dst, obj->format, file_line_buf.buf, function,
              level_string, message_buf.buf);
    }
  }
  cb_destroy(&file_line_buf);
  cb_destroy(&message_buf);
}
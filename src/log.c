/***************************************************************************//**

  @file         log.c

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Logging facilities implementation for libstephen.

  @copyright    Copyright (c) 2015-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include "libstephen/cb.h"
#include "libstephen/log.h"

/*
  This is the "permanent" default logger.  Even when you set your own default
  logger, this one still hangs around, waiting for you to call
  sl_set_default_logger(NULL).  It's declared here, but the default logger
  should go to standard out.  Unfortunately, you can't statically declare an
  instance of a struct that uses the variable stdout, so this is declared
  without any handlers, and the first time the default logger is referenced, the
  default log handler is added.
 */
static smb_logger default_logger = {
  .format = SMB_DEFAULT_LOGFORMAT,
  .num = 0
};

/*
  The only reason this pointer exists is so that reference_logger() knows
  whether or not the default logger has been set up yet.  This variable remains
  NULL until the first time the default logger is used, at which point the
  default handler is added to it, and then pdefault_logger is set to
  &default_logger.
 */
static smb_logger *pdefault_logger = NULL;

static char *level_names[] = {"NOTSET", "DEBUG", "INFO",
                              "WARNING", "ERROR", "CRITICAL"};

/*
  This function is used by most sl_ functions to initialize their `obj` pointer.
  Essentially, if obj=NULL, it replaces obj with a pointer to the default
  logger.  If the default logger hasn't been set up, it sets it up.
 */
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

void sl_destroy(smb_logger *obj) {
  (void)obj; // unused
  // nothing to delete
}

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

/*
  Returns a string representing a log level.  Very much not thread safe (but in
  reality, I don't think anything about this logger library is thread safe, so
  whatever ¯\_(ツ)_/¯ ).
 */
static char *sl_level_string(int level) {
  static char buf[20]; // plenty of space, to be safe.
  if (level % 10 == 0 && level >= LEVEL_NOTSET && level <= LEVEL_CRITICAL) {
    return level_names[level / 10];
  } else {
    snprintf(buf, 20, "%d", level);
    return buf;
  }
}

bool sl_will_log(smb_logger *obj, int level)
{
  for (int i = 0; i < obj->num; i++) {
    if (obj->handlers[i].level <= level) {
      return true;
    }
  }
  return false;
}

void sl_log(smb_logger *obj, char *file, int line, const char *function, int level, ...) {
  cbuf file_line_buf, message_buf;
  char *level_string, *format;
  va_list va;
  int i;

  reference_logger(&obj);

  if (!sl_will_log(obj, level)) {
    return; // early termination to prevent formatting if we can avoid it
  }

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

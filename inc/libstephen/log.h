/***************************************************************************//**

  @file         log.h

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Logging facilities for libstephen.

  @copyright    Copyright (c) 2015, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_LOG_H
#define LIBSTEPHEN_LOG_H

#include "base.h"

#define LEVEL_NOTSET    0
#define LEVEL_DEBUG    10
#define LEVEL_INFO     20
#define LEVEL_WARNING  30
#define LEVEL_ERROR    40
#define LEVEL_CRITICAL 50

#define SMB_MAX_LOGHANDLERS 10

#define SMB_DEFAULT_LOGLEVEL  LEVEL_NOTSET
#define SMB_DEFAULT_LOGDEST   stderr
#define SMB_DEFAULT_LOGFORMAT "%s: (%s) %s: %s\n"

typedef struct {

  int level;
  FILE *dst;

} smb_loghandler;

typedef struct {

  smb_loghandler handlers[SMB_MAX_LOGHANDLERS];
  char *format;
  int num;

} smb_logger;

void sl_init(smb_logger *obj);
smb_logger *sl_create();
void sl_destroy(smb_logger *obj);
void sl_delete(smb_logger *obj);

void sl_set_level(smb_logger *l, int level);
void sl_add_handler(smb_logger *l, smb_loghandler h, smb_status *status);
void sl_clear_handlers(smb_logger *l);
void sl_set_default_logger(smb_logger *l);

void sl_log(smb_logger *l, char *file, int line, const char *function, int level, ...);

#define LOG(logger, level, ...) sl_log(logger, __FILE__, __LINE__, __func__, level, __VA_ARGS__);

#define LDEBUG(...)            LOG(NULL, LEVEL_DEBUG,    __VA_ARGS__)
#define LINFO(...)             LOG(NULL, LEVEL_INFO,     __VA_ARGS__)
#define LWARNING(...)          LOG(NULL, LEVEL_WARNING,  __VA_ARGS__)
#define LERROR(...)            LOG(NULL, LEVEL_ERROR,    __VA_ARGS__)
#define LCRITICAL(...)         LOG(NULL, LEVEL_CRITICAL, __VA_ARGS__)

#endif // LIBSTEPHEN_LOG_H

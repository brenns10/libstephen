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

#define LNOTSET    0
#define LDEBUG    10
#define LINFO     20
#define LWARNING  30
#define LERROR    40
#define LCRITICAL 50

#define SMB_MAX_LOGHANDLERS 10

#define SMB_DEFAULT_LOGLEVEL  LNOTSET
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

#define DEBUG(...)            LOG(NULL, LDEBUG,    __VA_ARGS__)
#define INFO(...)             LOG(NULL, LINFO,     __VA_ARGS__)
#define WARNING(...)          LOG(NULL, LWARNING,  __VA_ARGS__)
#define ERROR(...)            LOG(NULL, LERROR,    __VA_ARGS__)
#define CRITICAL(...)         LOG(NULL, LCRITICAL, __VA_ARGS__)

#endif // LIBSTEPHEN_LOG_H

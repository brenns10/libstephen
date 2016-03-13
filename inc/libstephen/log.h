/***************************************************************************//**

  @file         libstephen/log.h

  @author       Stephen Brennan

  @date         Created Sunday, 24 May 2015

  @brief        Logging facilities for libstephen.

  @copyright    Copyright (c) 2015-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#ifndef LIBSTEPHEN_LOG_H
#define LIBSTEPHEN_LOG_H

#include "base.h"

/**
   @brief Most fine grained log level -- loggers will report everything.
 */
#define LEVEL_NOTSET    0
/**
   @brief Suggested log level for messages useful for debugging.
 */
#define LEVEL_DEBUG    10
/**
   @brief Suggested log level for informational messages.
 */
#define LEVEL_INFO     20
/**
   @brief Suggested log level for warning messages.
 */
#define LEVEL_WARNING  30
/**
   @brief Suggested log level for error messages.
 */
#define LEVEL_ERROR    40
/**
   @brief Suggested log level for critical, non-recoverable error messages.
 */
#define LEVEL_CRITICAL 50

/**
   @brief Max number of log handlers to be held by a logger.
 */
#define SMB_MAX_LOGHANDLERS 10

/**
   @brief The level that the default logger starts at.
 */
#define SMB_DEFAULT_LOGLEVEL  LEVEL_NOTSET
/**
   @brief The file that the default logger points at.
 */
#define SMB_DEFAULT_LOGDEST   stderr
/**
   @brief The format string for default loggers.

   This is kinda obnoxious because you can only change the formatting
   characters.  The order of the arguments is set in stone, and you can't change
   that with a format string.  The order is filename:line, function, level,
   message.
 */
#define SMB_DEFAULT_LOGFORMAT "%s: (%s) %s: %s\n"

/**
   @brief Specifies a log handler.

   This struct contains the information necessary to deliver a log message to
   its destination.  It has no memory management functions, since you should
   just pass it by value.
 */
typedef struct {

  /**
     @brief The minimum level this log handler accepts.
   */
  int level;
  /**
     @brief The destination file to send to.
   */
  FILE *dst;

} smb_loghandler;

/**
   @brief Specifies a logger.

   This struct represents a logger.  You can create it dynamically, but it is
   entirely possible to create a "literal" for it and declare it statically.
 */
typedef struct {

  /**
     @brief List of log handlers.
     @see SMB_MAX_LOGHANDLERS
   */
  smb_loghandler handlers[SMB_MAX_LOGHANDLERS];
  /**
     @brief Format string to use for this logger.
   */
  char *format;
  /**
    @brief The number of handlers actually contained in the handlers array.
   */
  int num;

} smb_logger;

/**
   @brief Initialize a logger object.
   @param obj Logger to initialize.
 */
void sl_init(smb_logger *obj);
/**
   @brief Allocate and return a logger object.
   @returns New logger.
 */
smb_logger *sl_create();
/**
   @brief Free resources held by a logger object.
   @param obj Logger to deinitialize.
 */
void sl_destroy(smb_logger *obj);
/**
   @brief Free resources and deallocate a logger object.
   @param obj Logger to deallocate.
 */
void sl_delete(smb_logger *obj);

/**
   @brief Set the minimum level this logger will report.

   All loghandlers have levels, but log messages are first filtered by the log
   level of the logger.  If the logger is set to have a higher level, then
   messages won't even get sent to the handler.  The level is inclusive.
   @param l The logger to set the level of. (NULL for default)
   @param level The level to set.  Can be any integer, but specifically one of
   the `LEVEL_*` constants would be good.
 */
void sl_set_level(smb_logger *l, int level);
/**
   @brief Add a log handler to the logger.

   Loggers have limited space for loghandlers, so this could fail.  Loggers are
   not intended to be array lists containing arbitrary amounts of handlers, and
   they're not intended to provide convenient access, update, or removal.
   @param l Logger to add to. (NULL for default)
   @param h Handler to add to the logger.
   @param[out] status For error reporting.
   @exception SMB_INDEX_ERROR If there is no more space for any more handlers.
 */
void sl_add_handler(smb_logger *l, smb_loghandler h, smb_status *status);
/**
   @brief Remove all handlers from the logger.
   @param l Logger to clear out. (NULL for default)
 */
void sl_clear_handlers(smb_logger *l);
/**
   @brief Set the static default logger to be your own.

   When you use the `D*` logger macros, the default logger is the one you log
   to.  The default logger will report everything to stderr, unless you modify
   it or set the default logger to a new one.
   @param l New logger.  NULL to reset it to the original one.
 */
void sl_set_default_logger(smb_logger *l);

/**
   @brief Log a message.

   This is the function that actually carries out any logging operation.  All
   the logging macros expand to a call to this function.  You really don't want
   to call this function directly, cause you have to provide a ton of
   information that you can't keep up to date in code, and it can all be
   provided by preprocessor macros.
   @param l Logger to send log message to.
   @param file Name of the code file the message originated from.
   @param line Line number of the log message.
   @param function Name of the function the message came from.
   @param level The log level of this message.
   @param ... Format string and format arguments, forming the actual message.
 */
void sl_log(smb_logger *l, char *file, int line, const char *function, int level, ...);

/**
   @brief Log a message.

   This macro fills out the file, line, and function parameters of sl_log() so
   that you only need to figure out the logger, level, and message.
   @param logger Logger to log to.
   @param level Level the message is at.
   @param ... Format string and format arguments.
 */
#define LOG(logger, level, ...) sl_log(logger, __FILE__, __LINE__, __func__, level, __VA_ARGS__);

/**
   @brief Log to default logger at LEVEL_DEBUG.
   @param ... Format string and format arguments.
 */
#define DDEBUG(...)            LOG(NULL, LEVEL_DEBUG,    __VA_ARGS__)
/**
   @brief Log to default logger at LEVEL_INFO.
   @param ... Format string and format arguments.
 */
#define DINFO(...)             LOG(NULL, LEVEL_INFO,     __VA_ARGS__)
/**
   @brief Log to default logger at LEVEL_WARNING.
   @param ... Format string and format arguments.
 */
#define DWARNING(...)          LOG(NULL, LEVEL_WARNING,  __VA_ARGS__)
/**
   @brief Log to default logger at LEVEL_ERROR.
   @param ... Format string and format arguments.
 */
#define DERROR(...)            LOG(NULL, LEVEL_ERROR,    __VA_ARGS__)
/**
   @brief Log to default logger at LEVEL_CRITICAL.
   @param ... Format string and format arguments.
 */
#define DCRITICAL(...)         LOG(NULL, LEVEL_CRITICAL, __VA_ARGS__)

/**
   @brief Log to any logger at LEVEL_DEBUG.
   @param logger Logger to log to.
   @param ... Format string and format arguments.
 */
#define LDEBUG(logger, ...)    LOG(logger, LEVEL_DEBUG,    __VA_ARGS__)
/**
   @brief Log to any logger at LEVEL_INFO.
   @param logger Logger to log to.
   @param ... Format string and format arguments.
 */
#define LINFO(logger, ...)     LOG(logger, LEVEL_INFO,     __VA_ARGS__)
/**
   @brief Log to any logger at LEVEL_WARNING.
   @param logger Logger to log to.
   @param ... Format string and format arguments.
 */
#define LWARNING(logger, ...)  LOG(logger, LEVEL_WARNING,  __VA_ARGS__)
/**
   @brief Log to any logger at LEVEL_ERROR.
   @param logger Logger to log to.
   @param ... Format string and format arguments.
 */
#define LERROR(logger, ...)    LOG(logger, LEVEL_ERROR,    __VA_ARGS__)
/**
   @brief Log to any logger at LEVEL_CRITICAL.
   @param logger Logger to log to.
   @param ... Format string and format arguments.
 */
#define LCRITICAL(logger, ...) LOG(logger, LEVEL_CRITICAL, __VA_ARGS__)

#endif // LIBSTEPHEN_LOG_H

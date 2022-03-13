#pragma once

#include "../platform.hpp"

#define LOG_WARN_ENABLED TRUE
#define LOG_INFO_ENABLED TRUE
#define LOG_DEBUG_ENABLED TRUE
#define LOG_TRACE_ENABLED TRUE

//Disable debug and trace logging for release builds.
#if RELEASE == TRUE
#define LOG_DEBUG_ENABLED FALSE
#define LOG_TRACE_ENABLED FALSE
#endif

typedef enum Log_Level
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} Log_Level;

bool8 initialize_logging();
void shutdown_logging();

LIBEXP void log_output(Log_Level level, const char *message, ...);

//#################################################################################
// FATAL.
//#################################################################################

// Logs a Fatal-Level Message.
#define LFATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

//#################################################################################
// ERROR.
//#################################################################################

#ifndef LERROR
// Logs a Error-Level Message.
#define LERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

//#################################################################################
// WARN.
//#################################################################################

#if LOG_WARN_ENABLED == TRUE
// Logs a Warn-Level Message.
#define LWARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
// Does nothing.
#define LWARN(message, ...)
#endif

//#################################################################################
// INFO.
//#################################################################################

#if LOG_INFO_ENABLED == TRUE
// Logs a Info-Level Message.
#define LINFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
// Does nothing.
#define LINFO(message, ...)
#endif

//#################################################################################
// DEBUG.
//#################################################################################

#if LOG_DEBUG_ENABLED == TRUE
// Logs a Debug-Level Message.
#define LDEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
// Does nothing.
#define LDEBUG(message, ...)
#endif

//#################################################################################
// TRACE.
//#################################################################################

#if LOG_TRACE_ENABLED == TRUE
// Logs a Trace-Level Message.
#define LTRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
// Does nothing.
#define LTRACE(message, ...)
#endif

void platform_console_write(const char* message, u8 colour);
void platform_console_write_error(const char* message, u8 colour);
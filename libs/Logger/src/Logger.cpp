
#include <include/Logger.hpp>
#include <assert.h>

// TODO: Temporary.
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//-------------------------------------------------------------------------------------------------------------
#if LINUX_PLATFORM
    void platform_console_write(const char* message, u8 colour) {
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        const char* colour_strings[6] = { "0;41", "1;31", "1;33", "1:32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colour_strings[colour], message);
    }

    void platform_console_write_error(const char* message, u8 colour) {
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        const char* colour_strings[6] = { "0;41", "1;31", "1;33", "1:32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colour_strings[colour], message);
    }

//-------------------------------------------------------------------------------------------------------------
#elif WINDOWS_PLATFORM
    void platform_console_write(const char* message, u8 colour) {
        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(console_handle, levels[colour]);

        OutputDebugStringA(message);
        u64 length = strlen(message);
        LPDWORD numberr_written = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, numberr_written, 0);
    }

    void platform_console_write_error(const char* message, u8 colour) {
        HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(console_handle, levels[colour]);

        OutputDebugStringA(message);
        u64 length = strlen(message);
        LPDWORD numberr_written = 0;
        WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, numberr_written, 0);
    }

//-------------------------------------------------------------------------------------------------------------
#elif APPLE_PLATFORM
    void platform_console_write(const char* message, u8 colour) {
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        const char* colour_strings[6] = { "0;41", "1;31", "1;33", "1:32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colour_strings[colour], message);
    }

    void platform_console_write_error(const char* message, u8 colour) {
        // FATAL, ERROR, WARN, INFO, DEBUG, TRACE.
        const char* colour_strings[6] = { "0;41", "1;31", "1;33", "1:32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colour_strings[colour], message);
    }
#endif

void report_assertion_failure(const char *expression, const char *message, const char *file, i32 line) {
    log_output(LOG_LEVEL_FATAL, "Assertion Failure: %s, message: %s, in file: %s, line: %d\n", expression, message, file, line);
}

bool8 initialize_logging() {
    //TODO: Create log file.
    return TRUE;
}

void shutdown_logging() {
    //TODO: Cleanup logging/write queued entries.
}

void log_output(Log_Level level, const char* message, ...) {
    const char *level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: "};
    bool8 error = level < LOG_LEVEL_WARN;

    const i32 msg_length = 32000;
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    __builtin_va_list args_ptr;
    va_start(args_ptr, message);
    vsnprintf(out_message, msg_length, message, args_ptr);
    va_end(args_ptr);

    char out_message_2[msg_length];
    if(snprintf(out_message_2, msg_length, "%s%s\n", level_strings[level], out_message) <= 0)
        snprintf(out_message_2, msg_length, "%s%s", level_strings[LOG_LEVEL_ERROR], "Mensaje excedio limites del buffer!\n");

    // Platform specific output.
    if(error) {
        platform_console_write_error(out_message_2, level);
    } else {
        platform_console_write(out_message_2, level);
    }
}
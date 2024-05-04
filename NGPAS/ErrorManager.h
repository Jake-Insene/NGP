#pragma once
#include <stdio.h>
#include <stdarg.h>

struct ErrorManager {
    static inline void error(const char* file_path, u32 line, u32 column, const char* format, ...) {
        va_list args;
        va_start(args, format);
        errorV(file_path, line, column, format, args);
        va_end(args);
    }
    
    static inline void errorV(const char* file_path, u32 line, u32 column, const char* format, va_list args) {
        printf("Error at %s:[%d, %d]:\n\t", file_path, line, column);
        vprintf(format, args);
        putchar('\n');
        error_count++;

        is_panic_mode = true;
        must_syncronize = true;
    }

    static inline void global_error(const char* file_path, const char* format, ...) {
        va_list args;
        va_start(args, format);
        global_errorV(file_path, format, args);
        va_end(args);
    }

    static inline void global_errorV(const char* file_path, const char* format, va_list args) {
        printf("Error at %s:\n\t", file_path);
        vprintf(format, args);
        putchar('\n');
        error_count++;

        is_panic_mode = true;
        must_syncronize = true;
    }

    static inline void warn(const char* file_path, u32 line, u32 column, const char* format, ...) {
        va_list args;
        va_start(args, format);
        errorV(file_path, line, column, format, args);
        va_end(args);
    }

    static inline void warnV(const char* file_path, u32 line, u32 column, const char* format, va_list args) {
        printf("Warn at %s:[%d, %d]:\n\t", file_path, line, column);
        vprintf(format, args);
        putchar('\n');
        error_count++;
    }

    static inline u32 error_count = 0;
    static inline u32 warning_count = 0;
    static inline bool is_panic_mode = false;
    static inline bool must_syncronize = false;
};

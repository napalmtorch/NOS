#pragma once
#include <lib/stdarg.h>
#include <lib/stdint.h>
#include <lib/stddef.h>

/// @brief Print formatted string @param fmt Pointer to formatted string @param args Argument list @return Result
int vprintf(const char* fmt, va_list args);

/// @brief Print formatted string @param fmt Pointer to formatted string @return Result
int printf(const char* fmt, ...);

int scanf(const char* fmt, ...);
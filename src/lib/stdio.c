#include <lib/stdio.h>
#include <kernel/hal/common/serial.h>

int vprintf(const char* str, va_list args)
{
    serial_vprintf(str, args);
}

int printf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    return 0;
}

int scanf(const char* fmt, ...)
{
    return 0;
}
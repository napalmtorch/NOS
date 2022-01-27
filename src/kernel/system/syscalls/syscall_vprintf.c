#include <kernel/hal/interrupts/syscall.h>
#include <kernel/core/kernel.h>

void syscall_vprintf(syscall_args_t args)
{
    char* str = args.src;
    va_list* va_args = args.arg;
    debug_vprintf(str, *va_args);
}
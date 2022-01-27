#include <kernel/hal/interrupts/syscall.h>
#include <kernel/core/kernel.h>

void syscall_methods(syscall_args_t args)
{
    // fetch method table
    uint32_t count = KERNEL_OBJDUMP.method_count;
    uint32_t methods = (uint32_t)KERNEL_OBJDUMP.methods;

    // set return pointers
    uint32_t* dest = args.dest;
    uint32_t* arg  = args.arg;
    dest[0]        = methods;
    arg[0]         = count;
}
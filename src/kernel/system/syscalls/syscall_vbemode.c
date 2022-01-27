#include <kernel/hal/interrupts/syscall.h>
#include <kernel/core/kernel.h>

void syscall_vbemode(syscall_args_t args)
{
    uint32_t* dest = args.dest;
    dest[0] = MBOOT_HDR->vbe_mode_info;
}
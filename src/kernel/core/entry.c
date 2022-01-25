#include <lib/stdint.h>
#include <kernel/core/kernel.h>

void _kernel_entry(multiboot_t* mboot)
{
    MBOOT_HDR = mboot;
    kernel_bootstage_0();
    kernel_bootstage_1();

    while (true) { kernel_main(); }
}
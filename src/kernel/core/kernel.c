#include <kernel/core/kernel.h>

extern uint32_t _boot_page_dir0;
extern uint32_t _boot_page_dir0_end;

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t _stack_top;
extern uint32_t _stack_bottom;

multiboot_t* MBOOT_HDR;
process_t*   PROC_KERNEL;

void kernel_bootstage_0()
{
    // initialize serial and debugging
    debug_toggle(true);
    serial_setport(SERIALPORT_COM1);

    // print boot information
    debug_info("Starting NapalmOS");
    debug_info("Bootloader   - MODS: %d, NAME: '%s'", MBOOT_HDR->modules_count, MBOOT_HDR->bootloader_name);
    debug_info("Kernel code  - VIRT: 0x%8x, PHYS: 0x%8x - 0x%8x", VIRT_KERNEL, kernel_get_physical(), kernel_get_physical_end());
    debug_info("Kernel stack - VIRT: 0x%8x, PHYS: 0x%8x - 0x%8x", stack_get_bottom() + VIRT_KERNEL, stack_get_bottom(), stack_get_top());

    // initialize gdt
    gdt_init();
    idt_init();
    _sti();

    // initialize physical memory management
    pmm_init();

    // initialize virtual memory management
    pfa_init_kernel();
    vmm_init();
    pfa_init(32 * MB);

    // initialize heaps
    heap_init(&HEAP_KERNEL, VIRT_KHEAP, 64  * MB, 4096);
    heap_init(&HEAP_USER,   VIRT_UHEAP, 512 * MB, 8192);

    // initialize process manager
    procmgr_init();

    // initialize storage drive manager
    vdrivemgr_init();

    // initialize ramdisk drive
    vfs_t* ramvfs = kmalloc(sizeof(vfs_t), HEAPTYPE_OBJECT);
    ramvfs->type = VFSTYPE_RAMFS;
    vdrive_t* ramdrive = vdrive_init("RAMDISK", 'A', VDRIVETYPE_RAMDISK, ramvfs);
    vfs_init(ramvfs, ramvfs->type);

    // initialize programmable interval timer
    pit_init(5000, pit_callback);
}

void kernel_bootstage_1()
{
    debug_ok("Finished booting");
    procmgr_ready();
}

uint32_t ksec, ksec_last, frames, fps;
void kernel_main()
{   
    frames++;
    ksec = pit_get_seconds();
    if (ksec != ksec_last)
    {
        fps = frames;
        frames = 0;
        ksec_last = ksec;
        debug_info("KERNEL TPS: %d", fps);
    }

    procmgr_schedule(false);
}

int idle_main(char** argv, int argc) 
{ 
    debug_info("Started idle thread");
    while (true) { procmgr_schedule(false); }
}

uint32_t kernel_get_physical() { return (uint32_t)&_kernel_start; }

uint32_t kernel_get_physical_end() { return mem_align(((uint32_t)&_kernel_end) - VIRT_KERNEL, 0x1000); }

uint32_t kernel_get_size() { return mem_align(kernel_get_physical_end() - kernel_get_physical(), 0x1000); }

uint32_t stack_get_top() { return ((uint32_t)&_stack_top) - VIRT_KERNEL; }

uint32_t stack_get_bottom() { return ((uint32_t)&_stack_bottom) - VIRT_KERNEL; }

uint32_t stack_get_size() { return stack_get_top() - stack_get_bottom(); }
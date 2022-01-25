#pragma once
#include <lib/ctype.h>
#include <lib/math.h>
#include <lib/stdarg.h>
#include <lib/stddef.h>
#include <lib/stdint.h>
#include <lib/stdio.h>
#include <lib/stdlib.h>
#include <lib/string.h>
#include <lib/time.h>
#include <lib/graphics/color.h>
#include <kernel/exec/elf.h>
#include <kernel/exec/exec.h>
#include <kernel/exec/process.h>
#include <kernel/exec/thread.h>
#include <kernel/memory/memory.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pfa.h>
#include <kernel/memory/heap.h>
#include <kernel/system/multiboot.h>
#include <kernel/system/debug.h>
#include <kernel/hal/common/ports.h>
#include <kernel/hal/common/serial.h>
#include <kernel/hal/common/registers.h>
#include <kernel/hal/common/pit.h>
#include <kernel/hal/common/atapio.h>
#include <kernel/hal/interrupts/gdt.h>
#include <kernel/hal/interrupts/idt.h>
#include <kernel/fs/vdrive.h>
#include <kernel/fs/vfs.h>
#include <kernel/fs/pmfs.h>
#include <kernel/fs/ramfs.h>

extern multiboot_t* MBOOT_HDR;
extern process_t*   PROC_KERNEL;

void _kernel_entry(multiboot_t* mboot);

void kernel_bootstage_0();
void kernel_bootstage_1();
void kernel_main();

uint32_t kernel_get_physical();
uint32_t kernel_get_physical_end();
uint32_t kernel_get_size();

uint32_t stack_get_top();
uint32_t stack_get_bottom();
uint32_t stack_get_size();

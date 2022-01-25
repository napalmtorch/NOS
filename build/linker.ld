ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
    _kernel_start = .;

    . = 0x00100000;

    .multiboot.text :
    {
        *(.multiboot.text)
    }

    . += 0xC0000000;
	
	.text : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	.data ALIGN (4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.rodata ALIGN (4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.bss : AT (ADDR (.bss) - 0xC0000000)
	{
       *(COMMON)
       *(.bss)
	}

	_kernel_end = .;
}
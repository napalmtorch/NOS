OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

address = 0x40000000;

SECTIONS
{	
	.text address : AT(address)
	{
		code = .;
		*(.text)
		*(.rodata)
		*(.rodata*)
		. = ALIGN(4096);
	}

	.data : AT(address + (data - code))
	{
		data = .;
		*(.data)
		. = ALIGN(4096);
	}

	.bss : AT(address + (bss - code))
	{
		bss = .;
       *(.bss)
		. = ALIGN(4096);
	}

	end = .;
}
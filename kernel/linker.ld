ENTRY(init)

SECTIONS
{
	. = 0x1000000000;

	.text BLOCK(4K): ALIGN(4K)
	{
		*(.text)
	}

	.data BLOCK(4K): ALIGN(4K)
	{
		*(.data)
	}

	.rodata BLOCK(4K): ALIGN(4K)
	{
		*(.rodata)
	}

	.bss BLOCK(4K): ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
}

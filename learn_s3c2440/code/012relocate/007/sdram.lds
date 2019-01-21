SECTIONS {
	.text	0	: { * (.text)}
	.rodata 	: { * (.rodata)}
	.data 0x30000000 : AT (0x800) 
	{ 
		data_load_addr = LOADADDR(.data);
		. = ALIGN(4);
		data_start = .;
		*(.data)
		data_end = .;
	}
	.bss 	: 
	{ 
		. = ALIGN(4);
		bss_start = .;
		*(.bss) 
		bss_end = .;
		*(.COMMON)
	}

}
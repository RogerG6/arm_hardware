SECTIONS {
	. = 0x32000000;
	. = ALIGN(4);
	text_start = .;
	.text :{ * (.text)}
	
	. = ALIGN(4);
	.rodata 	: { * (.rodata)}
	
	. = ALIGN(4);
	.data : { *(.data)}
	
	. = ALIGN(4);
	.bss 	: 
	{ 
		bss_start = .;
		*(.bss) 
		bss_end = .;
		*(.COMMON)
	}

}
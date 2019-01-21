
# include "uart.h"



void und_func(const volatile unsigned int addr, const volatile unsigned int ins)
{
	puts("Und exception occurs!\n\r");
	puts("addr: 0x = ");
	hex_print(addr);
	puts("instruction: ");
	hex_print(ins);
	puts("is a bad code.\n\r");
}

void swi_func(unsigned int val)
{
	puts("Swi exception occurs!\n\r");
	GPFDAT = (7 << 4);
	if (val == 0)
		GPFDAT &= ~(1 << 4);
	else if (val == 1)
		GPFDAT &= ~(1 << 5);
	if (val == 2)
		GPFDAT &= ~(1 << 6);
}

void print_swi_val(const volatile unsigned int val)
{
	puts("SWI value: ")	;
	hex_print(val & ~0xff000000);  //忽略高8bit
}

void print1(void)
{
	puts("abc\n\r");
}


void print2(void)
{
	puts("edf\n\r");
}






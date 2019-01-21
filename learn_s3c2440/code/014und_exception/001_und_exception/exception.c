
# include "uart.h"



void und_func(const volatile unsigned int addr, const volatile unsigned int ins)
{
	puts("und exception occurs!\n\r");
	puts("addr: 0x = ");
	hex_print(addr);
	puts("instruction: ");
	hex_print(ins);
	puts("is a bad instruction!\n\r");
}


void print1(void)
{
	puts("abc\n\r");
}


void print2(void)
{
	puts("edf\n\r");
}






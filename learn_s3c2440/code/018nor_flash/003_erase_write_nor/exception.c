
# include "uart.h"
# include "s3c2440_soc.h"


void und_func(const volatile unsigned int addr, const volatile unsigned int cpsr)
{
	puts("Und exception occurs!\n\r");
	puts("addr: 0x = ");
	hex_print(addr);
	puts("CPSR: ");
	hex_print(cpsr);
}

void swi_func(const volatile unsigned int addr, const volatile unsigned int cpsr)
{
	puts("Swi exception occurs!\n\r");
	puts("addr: 0x = ");
	hex_print(addr);
	puts("CPSR: ");
	hex_print(cpsr);	
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









# include "init.h"
# include "uart.h"

char g_A = 'A'; 

int a = 0;
int b;
short int c = 0;
long d = 0;

int main(void)
{
	
	uart_init();
	

		putchar(g_A);
		g_A++; 
		puts("\n\ra = ");
		hex_print(a); 
		puts("c = ");
		hex_print(c);
		puts("d = ");
		hex_print(d);
		delay(1000000);


	return 0;
}







/*
运行结果：
A
a = 0x00000000
c = 0x00000000
d = 0x00000000

*/









# include "init.h"
# include "uart.h"

char g_A = 'A'; 
char g_a = 'a';
char g_num = '0';

int a = 0;
int b;
short int c = 0;
long d = 0;

int main(void)
{
	
	uart_init();
	

		putchar(g_A);
		g_A++;    
		hex_print(a); 
		hex_print(c);
		hex_print(d);
		delay(1000000);


	return 0;
}

/*
运行结果：
	A0x00000000
	0x00000000
	0x00000000
*/



# include "init.h"
# include "uart.h"
# include "s3c2440_soc.h"

char g_A = 'A'; 

int a = 0;
int b;
short int c = 0;
long d = 0;

int main(void)
{
	puts("Main start:\n\r");
	led_init();
	key_init();
	timer0_init();

	while (1)
	{
		putchar(g_A);
		g_A++; 
		delay(1000000);
	}

	return 0;
}







/*
运行结果：


*/






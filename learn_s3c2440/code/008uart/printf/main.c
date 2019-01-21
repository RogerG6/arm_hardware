//串口


# include "my_printf.h"
int main(void)
{
	unsigned char c;
	uart_init();
	
	puts("Hello world!!!\n\r");   //'\r'  回到行首
	printf_test();
	while (1)
	{
		c = getchar();
		if (c == '\n')
			putchar('\r');
		if (c == '\r')      //说明我的串口按回车是‘\r’
			putchar('\n');
		putchar(c);
	}
	return 0;
}

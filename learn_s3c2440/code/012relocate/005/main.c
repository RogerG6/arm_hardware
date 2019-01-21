


# include "init.h"
# include "uart.h"

char g_A = 'A'; 
char g_a = 'a';
char g_num = '0';

int a = 0;
int b;

int main(void)
{
	
	uart_init();
	

		putchar(g_A);
		g_A++;    
		hex_print(a);   //由运行结果可知初值为0的全局变量并不是0
		delay(1000000);


	return 0;
}

/*
运行结果：
	A0xD1991993*/



# include "init.h"
# include "uart.h"

char g_A = 'A'; 

int a = 0;
int b;
short int c = 0;
long d = 0;

extern void led_ctrl(void);

int main(void)
{
	
	while (1)
	{
		led_ctrl();
	}

	return 0;
}







/*
运行结果：


*/






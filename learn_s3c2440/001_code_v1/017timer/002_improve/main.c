


# include "init.h"
# include "uart.h"
# include "s3c2440_soc.h"

char g_A = 'A'; 

int a = 0;
int b;
short int c = 0;
long d = 0;

typedef void (*pfint)(void);
pfint init_func_arr[3] = {
	led_init,
	key_init,
	timer0_init
};                    /* 这里数组的个数不得大于实际项数，否则程序会出错 */
 
int main(void)
{
	int i;
	for (i = 0; i < 3; i++)
		init_func_arr[i]();

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






//三个LED流水灯
# include "s3c2440_soc.h"

void delay(volatile int d)
{
	while (d--);
}

/* 每10ms被调用一次     
 * 每500ms点一次灯
 */
void led_timer0_func(void)
{
	static int n = 0;
	static int cnt = 0;
	int tmp;
#if 1
	cnt++;
	if (cnt < 50)
		return;
	cnt = 0;
#endif

	n++;

	tmp = ~n;
	tmp &= 7;
	GPFDAT &= ~(7 <<4);
	GPFDAT |= (tmp << 4);
/*		
		GPFDAT |= (1 << (n - 1));
		if (n == 7)
			n = 4;  
		GPFDAT &= ~(1 << n);
		n++;
		
	static int cnt = 0;
	int tmp;

	cnt++;

	tmp = ~cnt;
	tmp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |= (tmp<<4); */

}

int led_init(void)
{
	//配置gpf4,gpf5,gpf6为输出引脚
	GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
/*	GPFDAT |= (1 << 6);
	GPFDAT |= (1 << 5);
	GPFDAT |= (1 << 4);  */

	register_timer("led", led_timer0_func);
}

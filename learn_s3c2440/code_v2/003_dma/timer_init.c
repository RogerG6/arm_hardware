
# include "s3c2440_soc.h"

void timer0_func(void)
{
	static int n = 5;

	
		/* 流水灯 */
		GPFDAT |= (1 << (n - 1));
		if (n == 7)
			n = 4;  
		GPFDAT &= ~(1 << n);
		n++;
		
/*	static int cnt = 0;
	int tmp;

	cnt++;

	tmp = ~cnt;
	tmp &= 7;
	GPFDAT &= ~(7<<4);
	GPFDAT |= (tmp<<4); */

}

void timer0_init(void)
{
	/* TCFG0, 设置timer input频率
	 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	 */
	TCFG0 = 99;
	/* TCFG1,  
	 *        MUX 0 设置为External TCLK0, Timer0 频率为0.25M*/
	 
	TCFG1 = 0;

	
	/* TCNTB0
     * TCMPB0   当TCNTB0 = TCMPB0，输出电平	
	 * TCNTO0	用于观察Timer0的值 */
	 TCNTB0 = 60000;
	
	/* TCON, 设置timer0为auto reload，manual update 并启动 */
	TCON |= (1 << 1);
	
	TCON &= ~(1 << 1);
	TCON |= ((1 << 0) | (1 << 3));

	register_irq(timer0_func, 10);
}

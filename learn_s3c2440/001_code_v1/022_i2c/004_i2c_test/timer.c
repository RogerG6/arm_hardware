
# include "s3c2440_soc.h"
# include "interrupt.h"
#define TIMER_NUM 32
#define NULL ((void *)0)


typedef void (*timer_isr)(void);
typedef struct timer_func {
	char * name;
	timer_isr pf;
}timer_func, * p_timer_func;

timer_func timer_arr[TIMER_NUM];


int register_timer(char * name, timer_isr pf)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!(timer_arr[i].pf))
		{
			timer_arr[i].name = name;
			timer_arr[i].pf = pf;
			return 0;
		}
	}
	return -1;
}

int unregister_timer(char * name)
{
	int i;
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (!strcmp(timer_arr[i].name, name))
		{
			timer_arr[i].name = NULL;
			timer_arr[i].pf = NULL;
			return 0;
		}
	}
	return -1;
}


void Timer_Isr(unsigned int irq)
{
	int i;
	
//	printf("timer = %d\n\r", TCNTO0);
	for (i = 0; i < TIMER_NUM; i++)
	{
		if (timer_arr[i].pf)
			timer_arr[i].pf();
	}
}



void timer0_init(void)
{
	/* TCFG0, 设置timer input频率
	 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	 *							   = 50000000/(49+1)/16
	 *							   = 62500
	 */
	TCFG0 = 49;

	/* TCFG1,  
	 */
	TCFG1 &= ~0xf;
	TCFG1 = 3;		/* MUX0: 1/16 */

	
	/* TCNTB0
     * TCMPB0   当TCNTB0 = TCMPB0，输出电平	
	 * TCNTO0	用于观察Timer0的值 */
	 TCNTB0 = 625;
	
	/* TCON, 设置timer0为auto reload，manual update 并启动 */
	TCON |= (1 << 1);
	
	TCON &= ~(1 << 1);
	TCON |= ((1 << 0) | (1 << 3));
	
	register_irq(Timer_Isr, 10);
//	printf("timer = %d\n\r", TCNTO0);
#if 1
	int i;
	for (i = 0; i < TIMER_NUM; i++)			/* 必须对数组初始化 */
	{
		timer_arr[i].name = NULL;
		timer_arr[i].pf = NULL;
	}
#endif
}

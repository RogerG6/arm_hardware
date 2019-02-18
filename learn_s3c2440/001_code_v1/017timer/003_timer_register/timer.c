# include "s3c2440_soc.h"
# include "uart.h"
# include "my_printf.h"
# define NULL ((void *)0)


typedef void (*timer_func)(void);

typedef struct timer_desc {
	char * name;
	timer_func pf;
}timer_desc, p_timer_desc;

timer_desc timer_arr[32];

int register_timer(char * name, timer_func pf)
{
	int i;
	for (i = 0; i < 32; i++)
	{	
		if (!timer_arr[i].pf)
		{
//			printf("register_timer中 i = %d\n\r", i);
			timer_arr[i].name = name;
			timer_arr[i].pf = pf;
			return 0;
		}
	}
	return -1;
}

void timer0_func(void)
{
	static int n = 5;

//	puts("001\n\r");
	
		/* 流水灯 */
		GPFDAT |= (1 << (n - 1));
		if (n == 7)
			n = 4;  
		GPFDAT &= ~(1 << n);
		n++;
		

}

void timer_irq(unsigned int irq)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		
//		puts("002\n\r");
		if(timer_arr[i].pf)
		{			
//			printf("timer_irq中 i = %d\n\r", i);
			timer_arr[i].pf();
		}
	}
}




void timer0_init(void)
{
	/* TCFG0, 设置timer input频率
	 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	 */
	TCFG0 = 99;
	
	/* TCFG1,  
	 * MUX 0 设置为External TCLK0, Timer0 频率为0.25M
	 */ 
	TCFG1 = 0;

	
	/* TCNTB0
     * TCMPB0   当TCNTB0 = TCMPB0，输出电平	
	 * TCNTO0	用于观察Timer0的值 
	 */
	 TCNTB0 = 60000;
	
	/* TCON, 设置timer0为auto reload，manual update 并启动 */	
	
	TCON |= (1 << 1);        /* manual update from  TCNTB0 */ 
	
	TCON &= ~(1 << 1);       /* must clear before next writing */
	TCON |= ((1 << 0) | (1 << 3));     /* auto reload TCNTB0, start timer0 */
	register_irq(timer_irq, 10);
	printf("timer = %d\n\r", TCNTO0);

	int i;
	for (i = 0; i < 32; i++)
	{
		timer_arr[i].name = NULL;
		timer_arr[i].pf = NULL;
	}
//	int ret;
	register_timer("led", timer0_func);
//	printf("ret_reg_timer = %d\n\r", ret);

}




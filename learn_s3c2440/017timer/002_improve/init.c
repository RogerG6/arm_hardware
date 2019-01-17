# include "init.h"
# include "interrupt.h"
//配置SDRAM，往里面写内容，从里面读内容，成功则led亮，否则不亮


void delay(volatile int d)
{
	while (d--);
}


//三个LED灯初始化
void led_init(void)
{
	//配置gpf4,gpf5,gpf6为输出引脚
	GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
	GPFDAT |= (1 << 6);
	GPFDAT |= (1 << 5);
	GPFDAT |= (1 << 4);

}



void sdram_init(void)
{
	
	/*配置BWSCON = 0x22000000,*/
	BWSCON = 0x22000000;
	
	/*配置BANKCON6 = 0x18001, Trcd = 20ns, SCAN(column addr bit) = 9 bit*/
	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	/*配置REFRESH ＝ 0x8404f5
	  refresh enable, auto refresh, Trp = 20ns, Tsrc = 5CLk, Refresh count = 0x4F5*/
	REFRESH = 0x8404f5;
	 
	/*配置BANKSIZE ＝ 0xb1, burst enable, SCKE_EN, SCLK_EN, BK76MAP = 64MB/64MB*/
	BANKSIZE = 0xb1;
	
	/*配置MRSRB6 = 0x00000020， CL = 2 CLK*/
	MRSRB6 = 0x20;
	MRSRB7 = 0x20;

}


void relocate(void)
{
	extern int text_start, bss_start, bss_end;   //这些变量在sdram.lds中定义
	volatile int * load_addr = (volatile int *)0;
	volatile int * dest_start = (volatile int *)&text_start;  //此处必须用&, 语法规定
	volatile int * dest_end  = (volatile int *)&bss_end;

	while (dest_start < dest_end)
	{
		*dest_start++ = *load_addr++;
	}
}

void clear_bss(void)
{
	extern int bss_start, bss_end;
	volatile int * start_addr = (volatile int *)&bss_start;
	volatile int * end_addr  = (volatile int *)&bss_end;

	while (start_addr <= end_addr)
	{
		*start_addr++ = 0;
	}
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
	TCON |= (1 << 1);        /* manual update from  TCNTB0 */ 
	
	TCON &= ~(1 << 1);       /* must clear before next writing */
	TCON |= ((1 << 0) | (1 << 3));     /* auto reload TCNTB0, start timer0 */
	register_irq(timer0_func, 10);
}

//设置按键中断使能
/* eint0--GPF0, eint2--GPF2, eint11--GPG3, eint19--GPG11 */
void key_init(void)
{
	//配置为中断引脚
	GPFCON &= ~((3 << 0) | (3 << 4));
	GPGCON &= ~((3 << 6) | (3 << 22));   /* s2, s3被配置为中断引脚 */
	GPFCON |= ((2 << 0) | (2 << 4));
	GPGCON |= ((2 << 6) | (2 << 22));    /* s4, s5被配置为中断引脚 */
	
	//使能中断引脚
	EINTMASK &= ~((1 << 11) | (1 << 19)); /* 使能eint11, 19 */
	
	
	/* 配置为双边触发，即按下和松开都会触发中断 */
	EXTINT0 |= ((7 << 0) | (7 << 8));    /* s2, s3被配置为双边触发 */
	EXTINT1 |= (7 << 12);    			 /* s4 被配置为双边触发 */
	EXTINT2 |= (7 << 12);    			 /* s5 被配置为双边触发 */
	
	register_irq(key_irq_func, 0);
	register_irq(key_irq_func, 2);
	register_irq(key_irq_func, 5);
	
}




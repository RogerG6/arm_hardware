# include "init.h"
# include "interrupt.h"





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


int IsBootFromNor(void)
{
	volatile unsigned int * p = (volatile unsigned int *)0;
	volatile unsigned int val = *p;              //先读出来，存放

	*p = 0x12345678;
	if (*p == 0x12345678)         //相等则为nand, 否则为nor
	{
		*p = val;
		return 0;
	}
	else
		return 1;
}


void relocate(void)
{
	extern int text_start, bss_start, bss_end;   //这些变量在sdram.lds中定义
	volatile int * load_addr = (volatile int *)0;
	volatile int * dest_start = (volatile int *)&text_start;  //此处必须用&, 语法规定
	volatile int * dest_end  = (volatile int *)&bss_start;
	
	int len = (volatile int)&bss_start - (volatile int)&text_start;
	if (IsBootFromNor())
	{
		while (dest_start < dest_end)
		{
			*dest_start++ = *load_addr++;
		}
	}
	else
	{
		nand_init();
		read_nand((unsigned int)load_addr, ( unsigned char *)dest_start, len);
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




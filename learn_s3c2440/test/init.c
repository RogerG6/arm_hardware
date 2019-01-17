# include "init.h"
//配置SDRAM，往里面写内容，从里面读内容，成功则led亮，否则不亮

void sdram_init(void)
{
	
	/*配置BWSCON = 0x22000000,*/
	BWSCON = 0x22000000;
	
	/*配置BANKCON6 = 0x18001, Trcd = 20ns, SCAN(column addr bit) = 9 bit*/
	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	/*配置REFRESH = 0x8404f5
	  refresh enable, auto refresh, Trp = 20ns, Tsrc = 5CLk, Refresh count = 0x4F5*/
	REFRESH = 0x8404f5;
	 
	/*配置BANKSIZE ??0xb1, burst enable, SCKE_EN, SCLK_EN, BK76MAP = 64MB/64MB*/
	BANKSIZE = 0xb1;
	
	/*配置MRSRB6 = 0x00000020??CL = 2 CLK*/
	MRSRB6 = 0x20;
	MRSRB7 = 0x20;

}


void relocate(void)
{
	extern int text_start, bss_start;   //这些变量在sdram.lds中定??
	volatile unsigned int * load_addr = (volatile unsigned int *)0;
	volatile unsigned int * dest_start = (volatile unsigned int *)&text_start;  //此处必须??, 语法规定
	volatile unsigned int * dest_end  = (volatile unsigned int *)&bss_start;
	
	
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





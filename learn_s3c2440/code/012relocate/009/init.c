# include "init.h"
//配置SDRAM，往里面写内容，从里面读内容，成功则led亮，否则不亮

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


void relocate(volatile unsigned int * src, volatile unsigned int * dest, volatile unsigned int len)
{
	unsigned int i = 0;
	while (i < len)
	{
		*dest++ = *src++;
		i += 4;
	}
}

void clear_bss(volatile unsigned int * src, volatile unsigned int * dest)
{
	while (src <= dest)
	{
		*src++ = 0;
	}
}





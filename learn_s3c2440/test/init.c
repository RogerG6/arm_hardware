# include "init.h"
//����SDRAM��������д���ݣ�����������ݣ��ɹ���led����������

void sdram_init(void)
{
	
	/*����BWSCON = 0x22000000,*/
	BWSCON = 0x22000000;
	
	/*����BANKCON6 = 0x18001, Trcd = 20ns, SCAN(column addr bit) = 9 bit*/
	BANKCON6 = 0x18001;
	BANKCON7 = 0x18001;

	/*����REFRESH = 0x8404f5
	  refresh enable, auto refresh, Trp = 20ns, Tsrc = 5CLk, Refresh count = 0x4F5*/
	REFRESH = 0x8404f5;
	 
	/*����BANKSIZE ??0xb1, burst enable, SCKE_EN, SCLK_EN, BK76MAP = 64MB/64MB*/
	BANKSIZE = 0xb1;
	
	/*����MRSRB6 = 0x00000020??CL = 2 CLK*/
	MRSRB6 = 0x20;
	MRSRB7 = 0x20;

}


void relocate(void)
{
	extern int text_start, bss_start;   //��Щ������sdram.lds�ж�??
	volatile unsigned int * load_addr = (volatile unsigned int *)0;
	volatile unsigned int * dest_start = (volatile unsigned int *)&text_start;  //�˴�����??, �﷨�涨
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





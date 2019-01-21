# include "init.h"

void nor_init(void)
{
	volatile char c;
	volatile int ch;
	/*配置BWSCOND = 0x2，data bus width for bank 0 is 16bit(read only)*/
	BWSCON = 0x4;
	
	/*配置BANKCON0 = 0x00000500, 读地址时间>70ns, HCLK = 100MHz, cycle = 10ns*/
	do{
		puts("Enter a number 5~7: ");
		c = getchar();
		putchar(c);
		putchar('\n');
		putchar('\r');
		ch = (volatile int)(c - '0');
	}while (ch < 5 || ch > 7);
	
	BANKCON0 = (ch << 8);
}





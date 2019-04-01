#include "s3c2440_soc.h"



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

int isBootFromNor(void)
{
	unsigned int * p = (unsigned int *)0;

	unsigned int val = *p;
	*p = 0x12345678;
	if (*p == 0x12345678)
	{
		*p = val;
		return 0;
	}
	else
		return 1;
}

void nand_init(void)
{
    /* 设置时序参数 */
    #define TACLS 0
    #define TWRPH0 1
    #define TWRPH1 0
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);

    /* 使能nand flash 控制器, 初始化ECC，禁止片选 */
    NFCONT = (1 << 4) | (3 << 0);    /* 必须用 ＝ ，否则read only */
}

void nand_select(void)
{
    /* enable nCE */
     NFCONT &= ~(1 << 1);
}

void nand_deselect(void)
{
    /* disable nCE */
    NFCONT |= (1 << 1);
}

//写命令
void nand_cmd(unsigned char c)
{
    volatile int i;
    NFCMMD = c;
    for (i = 0; i < 10; i++); /* 使I/O线上的命令稳定, 具体时间可参考相关latency参数 */
}

//写地址 
void nand_addr(unsigned char c)
{
    volatile int i;
    NFADDR = c;
    for (i = 0; i < 10; i++);  /* 使I/O线上的地址稳定 */

}
//读数据
unsigned char nand_data(void)
{
    return NFDATA;
}

void wait_to_ready(void)
{
    while(!(NFSTAT & 1));
}

void nand_read(unsigned int src, unsigned char * dest, unsigned int len)
{
	unsigned int page = src / 2048;
	unsigned int col = src % 2048;
	int i = 0;

	while (i < len)
	{
		/* 1、片选选中 */
		nand_select();

		/* 2、发出0x00命令 */
		nand_cmd(0x00);

		/* 3、发出地址（分5步发出） */
	    /* col addr */
	    nand_addr(col & 0xff);
	    nand_addr((col >> 8) & 0xff);

	    /* row addr */
	    nand_addr(page & 0xff);
	    nand_addr((page >> 8) & 0xff);
	    nand_addr((page >> 16) & 0xff);
		
		/* 4、发出0x30命令 */
		nand_cmd(0x30);
		wait_to_ready();
		
		/* 5、读数据 */
		for (; (col < 2048) && (i < len); col++)
		{
			dest[i++] = nand_data();
			src++;
		}
		if (col == 2048)
		{
			col = 0;
			page++;
		}

	}
	/* 6、取消选中 */
	nand_deselect();
}

void copy2sdram(unsigned char * src, unsigned char * dest, unsigned int len)
{
	int i = 0;
	
	if (isBootFromNor())
	{
		while (i < len)
		{
			dest[i] = src[i];
			i++;
		}
	}
	else
	{
		nand_read((unsigned int)src, dest, len);
	}
}

void clear_bss(void)
{
	/* 要从lds文件中获得 __bss_start, _end
	 */
	extern int _end, __bss_start;

	volatile unsigned int *start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *end = (volatile unsigned int *)&_end;


	while (start <= end)
	{
		*start++ = 0;
	}
}

//============================UART=====================================
//配置115200,8n1
void uart0_init(void)
{
	/* Transmit/Receive Mode：UCON0(0x50000004) = 0x5 */
	/* 配置GPH2, GPH3为TXD0，RXD0*/
	GPHCON &= ~((3 << 4) | (3 << 6));
	GPHCON |= ((2 << 4) | (2 << 6));
	GPHUP &= ~((1<<2) | (1<<3));    //使上拉电阻工作
	
	//设置baud rate  UBRDIVn= (int)( UART clock / ( buad rate x 16) ) –1
	UCON0 = 0x00000005;   //PCLK, 中断/查询模式
	UBRDIV0 = 26;
	
	/*设置T/R format   
	 * 8n1: ULCON0(0x50000000) = 0x3 */
	 ULCON0 = 0x00000003;
}

//向终端显示字符
void putchar(int c)  
{
	//UTRSTAT0
	while (!(UTRSTAT0 & (1 << 2)));
	UTXH0 = (unsigned char)c;
}

//从终端获取字符
int getchar(void)
{
	//URXH0
	while (!(UTRSTAT0 & (1 << 0)));
	return URXH0;
}

//在终端打印字符串
void puts(const char * s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
}

void puthex(unsigned int val)
{
	/* eg. 0x12345678 */
	int i;
	char c;

	puts("0x");
	for (i = 0; i < 8; i++)
	{
		c = (val >> 4 * (7 - i)) & 0xf;
		if ((c >= 0) && (c <= 9))
			putchar('0' + c);
		else
			putchar('A' + c - 10);
	}
	puts("\n\r");
}




 

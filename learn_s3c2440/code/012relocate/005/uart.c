# include "s3c2440_soc.h"

//配置115200,8n1
void uart_init()
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
int putchar(int c)  
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
int puts(const char * s)
{
	while (*s)
	{
		putchar(*s);
		s++;
	}
}

void hex_print(int n)
{
	char hex[8];
	int i;
	for (i = 0; i < 8; i++)
	{
		hex[i] = (n & 0xf);
		if (hex[i] >= 0 && hex[i] <= 9)
			hex[i] = hex[i] + '0';
		else if (hex[i] >= 10 && hex[i] <= 15)
			hex[i] = hex[i] - 10 + 'A';
		n >>= 4;
	}
	
	puts("0x");
	for ( i = 7; i >= 0; i--)
	{
		putchar(hex[i]);
	}
	puts("\n\r");
}

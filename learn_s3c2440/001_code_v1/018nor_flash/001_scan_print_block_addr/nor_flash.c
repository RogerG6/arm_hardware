# include "uart.h"
# include "my_printf.h"

# define NOR_BASE 0     /* jz2440 nor flash 基地址 */
/* 功能：识别
 * 厂家、型号、size、sectors、Manufacturer ID、Device ID、Erase timeout、Write timeout */

/* 向nor addr中写data
 * 由于nor与cpu地址线连接方式为A0-A1，因此，从cpu角度看：addr = (base + (offset << 1)) */
void wrt_word_nor(unsigned int base, unsigned int offset, unsigned int data)
{
	volatile unsigned short * p = (volatile unsigned short *)(base + (offset << 1));
	*p = data;
}

/* 封装 */
void nor_cmd(unsigned int offset, unsigned int data)
{
	wrt_word_nor(NOR_BASE, offset, data);
}

/* 从nor addr中读data */
unsigned short read_word_nor(unsigned int base, unsigned int offset)
{
	volatile unsigned short * p = (volatile unsigned short *)(base + (offset << 1));
	return *p;
}

/* 封装 */
unsigned short nor_dat(unsigned int offset)
{
	return read_word_nor(NOR_BASE, offset);
}


/* 读取nor flash各类信息，并打印 */
void scan_nor(void)
{
	int blocks, block_size, i, j;
	int base_addr, nor_base = 0, cnt = 0;		
	/* Manufacturer ID、Device ID
	 * 以下用assembly-like描述：
	 * 1. [555H] = aah
	 * 2. [2AAH] = 55h
	 * 3. [555H] = 90h
	 * 4. [0] = Manufacturer ID
		  [1] = Device ID
	 */
	 nor_cmd(0x555, 0xaa);
	 nor_cmd(0x2aa, 0x55);
	 nor_cmd(0x555, 0x90);
	 printf("Manufacturer ID: 0x%x, Device ID: 0x%x\n\r", nor_dat(0), nor_dat(1));
	 
	 /* 进入CFI模式：[55H] = 98h
	  * 读取size: 		[27h]
	  *		sectors :	[2c]
	  *	打印
	  */
	 nor_cmd(0x55, 0x98);
	 printf("Size: %d M, regions: %d\n\r", (1 << nor_dat(0x27))/1024/1024, nor_dat(0x2c));
	  
	  

	/* 打印各扇区起始地址 */
	base_addr = 0x2d;		//基地址 
	for (i = 0; i < 4; i++)   //4 regions
	{
		blocks = 1 + (nor_dat(base_addr) + (nor_dat(base_addr + 1) << 8));            //每个region中的扇区数
		block_size = 256 * (nor_dat(base_addr + 2) + (nor_dat(base_addr + 3) << 8));  //每个扇区的大小
		for (j = 0; j < blocks; j++)
		{
			printf("%08x  ", nor_base);
			cnt++;
			if (cnt % 5 == 0)
				puts("\n\r");
			nor_base += block_size;
		}
		base_addr += 4;
	}
	puts("\n\r");
	
	//退出CFI模式
	nor_cmd(0, 0xf0);
}

void nor_flash_test(void)
{
	puts("[s] Scan nor flash\n\r");
	puts("[e] Erase nor flash\n\r");
	puts("[w] Write nor flash\n\r");
	puts("[r] Read nor flash\n\r");
	puts("[q] Quit\n\r");
	
	
	scan_nor();
}








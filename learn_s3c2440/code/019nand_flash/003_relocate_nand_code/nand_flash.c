# include "s3c2440_soc.h"
# include "my_printf.h"
void nand_init(void)
{
    /* 设置时序参数 */
    # define TACLS 0
    # define TWRPH0 1
    # define TWRPH1 0
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);

    /* 使能nand flash 控制器, 初始化ECC，禁止片选 */
    NFCONT |= (1 << 4) | (3 << 0);
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

//读nand flash的芯片信息
void do_scan_nand(void)
{
    unsigned char buf[5] = {0};
    volatile int i;
    nand_select();

    /* read id */
    nand_cmd(0x90);
    nand_addr(0x00);
    for (i = 0; i < 5; i++)
        buf[i] = nand_data();

    nand_deselect();

    printf("Maker code:  0x%x\n\r", buf[0]);
    printf("Device code: 0x%x\n\r", buf[1]);
    printf("3rd cycle: 0x%x\n\r", buf[2]);
    printf("4th cycle: 0x%x\n\r", buf[3]);
    printf("Page Size:  %d KB\n\r", 1 << (buf[3] & 0x3));
    printf("Block Size: %d KB\n\r", 64 << ((buf[3] >> 4) & 0x3));
    printf("5th cycle: 0x%x\n\r", buf[4]);

    
}

//read nand flash 
void read_nand(unsigned int addr, unsigned char * dest, unsigned int len)
{
    int i = 0;
    int page = addr / 2048;             //row/page addr
    int col = addr & (2048 - 1);        //col addr

    nand_select();
    while (i < len)
    {
        
        /* 发出00h命令，read operation */
        nand_cmd(0x00);

        /* 发出地址 */
        /* col addr */
        nand_addr(col & 0xff);
        nand_addr((col >> 8) & 0xff);

        /* row addr */
        nand_addr(page & 0xff);
        nand_addr((page >> 8) & 0xff);
        nand_addr((page >> 16) & 0xff);

        /* 发出30h命令 */
        nand_cmd(0x30);

        /* 等待就绪，判断busy or  free*/
        wait_to_ready();

        for (; (col < 2048) && (i < len); col++)
        {
            dest[i++] = nand_data();
        }
        if (i == len)
            break;
        col = 0;
        page++;
    }

    nand_deselect();
}

void nand_flash_test(void)
{
	char c;
	nand_init();
    
	while(1)
	{
		puts("[s] Scan nand flash\n\r");
		puts("[e] Erase nand flash\n\r");
		puts("[w] Write nand flash\n\r");
		puts("[r] Read nand flash\n\r");
		puts("[q] Quit\n\r");
		puts("Enter your choice : ");
		c = getchar();
		putchar(c);
		puts("\n\r");
		switch (c)
		{
			case 's':
			case 'S': do_scan_nand(); break;
			
			case 'e':
			case 'E': //erase_nand(); break;

			case 'w':
			case 'W':// write_nand(); break;

			case 'r':
			case 'R': //read_nand(); break;

			case 'q':
			case 'Q': break;

		}


	}
}



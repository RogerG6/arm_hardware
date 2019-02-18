# include "nand_flash.h"
void nand_init(void)
{
    /* 设置时序参数 */
    # define TACLS 0
    # define TWRPH0 1
    # define TWRPH1 0
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

//写数据
void nand_write_data(unsigned char data)
{
    NFDATA = data;
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

/* 坏块判断 */
int nand_bad(unsigned int addr)
{
        int page = (addr / 0x20000) * 64;             //row/page addr
        int col = 2048;                       //col addr
        unsigned char val;
            nand_select();

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
            val = nand_data();
            nand_deselect();

            if (val != 0xff)     
                return 1;           //是坏块
            else    
                return 0;
}


//read nand flash 
void read_nand(unsigned int addr, unsigned char * dest, unsigned int len)
{
    volatile int i , page, col;
    volatile int flag = 1;
    i = 0;
         while (1)   
         {
            if (flag == 1 && nand_bad(addr))    //flag == 1: 128kb对齐
            {
//                puts("001\n\r");
                addr += 128 * 1024;
                addr = (addr / 0x20000) * 0x20000;    //确保读的是下一个block的首地址
                continue;
            }

            nand_select();
            page = addr / 2048;             //row/page addr
            col = addr & (2048 - 1);        //col addr

//            puts("002\n\r");
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
                    addr++;
                }
                if (i == len)
                    break;
                //col = 0;
                if (addr & 0x1ffff)
                    flag = 0;
                else    
                    flag = 1;
        } 
     nand_deselect();
}

//擦写nand flash
int erase_nand(unsigned int addr, unsigned int len)
{
    int page = addr / 2048;             //row/page addr

    if (len & 0x1ffff)
    {
        puts("nand erasing error, the len is not 128KB align\n\r");
        return -1;
    }
    else if (addr & 0x1ffff)
    {
        puts("nand erasing error, the address is not 128KB align\n\r");
        return -1;
    }
    nand_select();
    while (1)
    {
            //发出60h命令，Auto Block EraseSetup Command
            nand_cmd(0x60);

            /* 发出row addr */
            nand_addr(page & 0xff);
            nand_addr((page >> 8) & 0xff);
            nand_addr((page >> 16) & 0xff);

            //发出d0h命令，Erase Command
            nand_cmd(0xd0);

            wait_to_ready();

        len -= (128 * 1024);     //erase 以block为最小单位
        if (len == 0)
            break;
        addr += (128 * 1024);
    }
    nand_deselect();
    return 0;
}

//do_erase_nand func
void do_erase_nand(void)
{
    unsigned int addr;
    puts("Enter the nand flash address to erase: ");
    addr = get_uint();

    erase_nand(addr, 128 * 1024);
}

//write nand
void write_nand(unsigned int addr, unsigned char * buf, unsigned int len)
{
    int i = 0;
    int page ;
    int col, flag = 1; 

        while (1)
        {
            if (flag == 1 && nand_bad(addr))    //flag == 1: 128kb对齐
            {
                addr += 128 * 1024;
                addr = (addr / 0x20000) * 0x20000;    //确保写的是下一个block的首地址
                continue;
            }

            page = addr / 2048;             //row/page addr
            col = addr & (2048 - 1);        //col addr
            nand_select();
            //发出80h命令，serial data input command
            nand_cmd(0x80);

            //发出地址
            /* col addr */
            nand_addr(col & 0xff);
            nand_addr((col >> 8) & 0xff);

            /* row addr */
            nand_addr(page & 0xff);
            nand_addr((page >> 8) & 0xff);
            nand_addr((page >> 16) & 0xff);
            for (; (col < 2048) && (i < len); col++)
            {
                nand_write_data(buf[i++]);       //write 以 page 为最小单位
                addr++;
            }
            //发出10h命令，program command
            nand_cmd(0x10);
            wait_to_ready();
        
            if (len == i)
                break;
//            col = 0;
            if (addr & 0x1ffff)
                flag = 0;
            else
                flag = 1;       //addr 128KB对齐
        }
    nand_deselect();
}

//写nand flash
void do_write_nand(void)
{
    unsigned int addr;
    unsigned char str[100];

    puts("Enter the nand flash address to write:");
    addr = get_uint();
    puts("Enter the strings to write to nand flash:");
    gets(str);
 //   printf("%x, %s, %d\n\r", addr, str, strlen(str));
    write_nand(addr, str, strlen(str));
}

//do_read_nand func
void do_read_nand(void)
{
    unsigned char buf[64];
    unsigned int i, j, k = 0, m, addr, obb_addr;

    puts("Enter the address to read from nand flash: ");
    addr = get_uint();

        read_nand(addr, buf, 64);

        for (i = 0; i < 4; i++)
        {
            m = k;
            for (j = 0; j < 16; j++)
            {
                printf("%02x ", buf[k++]);
            }
            puts("   ");
            for (j = 0; j < 16; j++)
            {
                if (buf[m] < 0x20 || buf[m] > 0x7e)
                    putchar('.');
                else 
                    putchar(buf[m]);
                m++;
            }
            puts("\n\r");
        }
     puts("\n\r");
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
			case 'E': do_erase_nand(); break;

			case 'w':
			case 'W':do_write_nand(); break;

			case 'r':
			case 'R': do_read_nand(); break;

			case 'q':
			case 'Q': break;

		}


	}
}



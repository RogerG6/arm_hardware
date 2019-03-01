
void write_at24cxx(void)
{
	unsigned char str[10];
	unsigned int addr;
	int err;


	puts("Enter the address to write to at24cxx: ");
	addr = get_uint();
	
	puts("Enter the information: ");
	gets(str);

	err = at24cxx_write(addr, str, strlen(str) + 1);
	printf("write err = %d\n\r", err);
}


void read_at24cxx(void)
{
	unsigned int addr;
	char data[100];
	int i, j, cnt = 0;
	int index, len, err;
	
	puts("Enter the address to read from at24cxx: ");
	addr = get_uint();

	puts("Enter the length ro read: ");
	len = get_int();

	err = at24cxx_read(addr, data, len);
	printf("read err = %d\n\r", err);
		
	/* 打印的扇区大小固定为64byte */
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 16; j++)
		{
			printf("%02x ", data[cnt++]);
		}
		puts("   ");
		index = i * 16;
		for (j = 0; j < 16; j++)
		{
			if (data[index + j] < 0x20 || data[index + j] > 0x7e)
				putchar('.');
			else	
				putchar(data[index + j]);
		}
		puts("\n\r");
	}
	puts("\n\r");
}



void i2c_test(void)
{
	/* 初始化 */
	i2c_init();
	
	/* 提供选择菜单 */
	char c;
	while(1)
	{
		puts("[w] Write nor flash\n\r");
		puts("[r] Read nor flash\n\r");
		puts("[q] Quit\n\r");
		puts("Enter your choice : ");
		c = getchar();
		putchar(c);
		puts("\n\r");
		switch (c)
		{
			case 'w':
			case 'W': write_at24cxx(); break;

			case 'r':
			case 'R': read_at24cxx(); break;

			case 'q':
			case 'Q': break;
		}
	}	
}

# define GPFCON ((volatile unsigned int *)(0x56000050))
# define GPFDAT ((volatile unsigned int *)(0x56000054))

void delay(volatile int d)
{
	while (d--);
}

int main(int which)
{
	volatile unsigned int * pgpfcon = GPFCON;
	volatile unsigned int * pgpfdat = GPFDAT;
	//配置gpf4,gpf5,gpf6为输出引脚
	* pgpfcon &= ~((3 << 8) | (3 << 10) | (3 << 12));
	* pgpfcon |= ((1 << 8) | (1 << 10) | (1 << 12));
	* pgpfdat |= (7 << 4);
	
	int n = 4;
	while (1)
	{
		* pgpfdat &= ~(1 << n);
		delay(20000);
		* pgpfdat |= (1 << n);
		delay(20000);
		n++;
		if (n == 7)
			n = 4;
	}

	return 0;
}

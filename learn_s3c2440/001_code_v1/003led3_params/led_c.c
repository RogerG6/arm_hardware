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
	if (which == 4)
	{
		* pgpfdat &= ~(1 << 4);
	}
	else if (which == 5)
	{
		* pgpfdat &= ~(1 << 5);
	}
	else if (which == 6)
	{
		* pgpfdat &= ~(1 << 6);
	}
	
	return 0;
}

//三个LED流水灯

# define GPFCON *(((volatile unsigned int *)(0x56000050)))
# define GPFDAT *(((volatile unsigned int *)(0x56000054)))

void delay(volatile int d)
{
	while (d--);
}

int test_led(void)
{
	int val = 0;
	int ct = 0;   //计数器，循环次数
	int tmp;
	//配置gpf4,gpf5,gpf6为输出引脚
	GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
	
	
	while (ct < 10)
	{
		tmp = ~val;
		GPFDAT &= ~(7 << 4);
		GPFDAT |= (tmp << 4);
		delay(100000);
		val++;
		if (val == 8)
			val = 0;
		ct++;
	}
	return 0;
}

# include "s3c2440_soc.h"

void delay(volatile int d)
{
	while (d--);
}


//function: 按下哪个键，然后松开，哪个灯亮
int main(int which)
{
	int val1, val2;
	char flag1 = 1, flag2 = 1, flag3 = 1;
	//配置gpf4,gpf5,gpf6为输出引脚
	GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
	
	//配置gpf0,gpf2,gpg3为输入引脚
	GPFCON &= ~((3 << 0) | (3 << 4));
	GPGCON &= ~(3 << 6);
	
	
	while (1)
	{
		val1 = GPFDAT;
		val2 = GPGDAT;
		
		if (!(val1 & (1 << 0)))
		{
			while (!(val1 & (1 << 0)))
				val1 = GPFDAT;
			flag1 = -flag1;
		}
				
		
		if (!(val1 & (1 << 2)))
		{
			while (!(val1 & (1 << 2)))
				val1 = GPFDAT;
			flag2 = -flag2;
		}
		
		if (!(val2 & (1 << 3)))
		{
			while (!(val2 & (1 << 3)))
				val2 = GPGDAT;
			flag3 = -flag3;
		}
		
		
		if (flag1 == 1)
			GPFDAT |= (1 << 6);
		else
			GPFDAT &= ~(1 << 6);
		if (flag2 == 1)
			GPFDAT |= (1 << 5);
		else	
			GPFDAT &= ~(1 << 5);
		if (flag3 == 1)
			GPFDAT |= (1 << 4);
		else	
			GPFDAT &= ~(1 << 4);
	}
	return 0;
}

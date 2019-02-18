//三个LED流水灯
# include "s3c2440_soc.h"

void delay(volatile int d)
{
	while (d--);
}

int led_init(void)
{
	//配置gpf4,gpf5,gpf6为输出引脚
	GPFCON &= ~((3 << 8) | (3 << 10) | (3 << 12));
	GPFCON |= ((1 << 8) | (1 << 10) | (1 << 12));
	GPFDAT |= (1 << 6);
	GPFDAT |= (1 << 5);
	GPFDAT |= (1 << 4);

}

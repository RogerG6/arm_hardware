//nor_init_test


# include "init.h"
int main(void)
{
	unsigned char c;
	uart_init();
	sdram_init();
	
	volatile unsigned char * p = (volatile unsigned char *)0x30000000;
	int i;
	
	//write byte
	for (i = 0; i < 1000; i++)
		p[i] = 0x50;
	
	//read byte
	for (i = 0; i < 1000; i++)
	{
		if (p[i] != 0x50)
			return 0;
	}
	test_led();

	return 0;
}

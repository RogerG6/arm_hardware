//nor_init_test


# include "init.h"
int main(void)
{
	unsigned char c;
	uart_init();

	while (1)
	{
		nor_init();
		test_led();

	}
	return 0;
}




# include "init.h"
# include "uart.h"
# include "s3c2440_soc.h"


int main(void)
{
	puts("Main start:\n\r");
	key_init();
	timer0_init();
	led_init();
	puts("Lcd test start:\n\r");

	//lcd_test();
	//puts("ADC test:\n\r");
	//adc_test();
	
	puts("Touchscreen test start:\n\r");
	//touchscreen_test();

	puts("I2C test start:\n\r");
	i2c_test();

	while (1);
	return 0;
}







/*
运行结果：


*/






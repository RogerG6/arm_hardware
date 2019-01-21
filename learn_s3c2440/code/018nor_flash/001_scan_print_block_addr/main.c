


# include "init.h"
# include "uart.h"
# include "s3c2440_soc.h"


int main(void)
{
	
	  interrupt_ctrl_init();
	  key_int();
	  led_init();
	  //timer0_init();

	nor_flash_test();
	
	while(1);

	return 0;
}





/*
运行结果：


*/






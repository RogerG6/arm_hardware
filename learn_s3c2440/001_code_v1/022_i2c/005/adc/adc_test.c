
# include "../my_printf.h"


void adc_test(void)
{
	int data;
	double vol;	//电压
	int m;		//整数部分
	int n;		//小数部分

	/* 初始化adc */
	adc_init();

	while (1)	
	{
		/* 读数据 */
		data = adc_read();

		
		/* 将data进行格式转换，因为printf无法打印小数 */
		vol = data * 3.3 /1023; 	/* 1023 -- 3.03v */
		m = vol;					/* eg: 3.3v --> 3 */
		n = (vol - m) * 1000;		/* eg: 3.3v --> 30 */
		
		/* 在串口终端打印 */
		printf("Vol: %d.%03d\r", m, n);   /* eg: 3.3v --> 3.030 */

	}
}





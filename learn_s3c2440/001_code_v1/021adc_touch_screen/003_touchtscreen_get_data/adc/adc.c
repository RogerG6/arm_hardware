
#include "../s3c2440_soc.h"

void adc_init(void)
{
	/* [15]   : ECFLG, 1 = End of A/D conversion
	 * [14]   : PRSCEN, A/D converter prescaler enable
	 * [13:6] : PRSCVL, A/D converter prescaler value
	 			A/D converter freq = PCLK / (PRSCVL + 1)
	 							   = 50MHz / (49 + 1)
	 							   = 1 MHz
	 * [5:3]  : SEL_MUX, 000 = AIN 0
	 * [2]    : STDBM, Standby mode select
	 * [1]    : READ_ START
	 * [0]	  : ENABLE_START, 1 = A/D conversion starts and this bit is cleared after the startup
	 */
	ADCCON = (1 << 14) | (49 << 6) | (0 << 3);

	ADCDLY = 0xff;
}


int adc_read(void)
{
	/* 启动adc */
	ADCCON |= (1 << 0);

	/* 判断转换是否完成 */
	while (!(ADCCON & (1 << 15)));
	
	return ADCDAT0 & 0x3ff;
}


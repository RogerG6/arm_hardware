#include "../s3c2440_soc.h"
#include "../interrupt.h"
#include "../uart.h"

#define INT_ADC_S (10)
#define INT_TC	  (9)
 
#define PEN_UP   (1 << 8)
#define PEN_DOWN (0 << 8)

#define YM_ENABLE  (1 << 7)
#define YM_DISABLE (0 << 7)

#define YP_ENABLE  (0 << 6)
#define YP_DISABLE (1 << 6)

#define XM_ENABLE  (1 << 5)
#define XM_DISABLE (0 << 5)

#define XP_ENABLE  (0 << 4)
#define XP_DISABLE (1 << 4)

#define PULLUP_ENABLE  (0 << 3)
#define PULLUP_DISABLE (1 << 3)

#define AUTO_PST  (1 << 2)

#define WAIT_MODE (3)
#define NO_OPR	  (0)

void enter_wait_pen_down(void)
{	
	ADCTSC = PEN_DOWN | XM_DISABLE | XP_DISABLE | YP_DISABLE | YM_ENABLE | PULLUP_ENABLE | WAIT_MODE;
}

void enter_wait_pen_up(void)
{	
	ADCTSC = PEN_UP | XM_DISABLE | XP_DISABLE | YP_DISABLE | YM_ENABLE | PULLUP_ENABLE | WAIT_MODE;
}

void Isr_Tc(void)
{	
	hex_print(ADCDAT0);
	
	if (ADCDAT0 & (1 << 15))	/* pen up */
	{
		puts("pen up");
		enter_wait_pen_down();
	}
	else						/* pen down */
	{
		puts("pen down");
		enter_wait_pen_up();
	}
}

void AdcTsIntHandle(unsigned int irq)
{
	hex_print(SUBSRCPND);

	if (SUBSRCPND & (1 << INT_TC)) /* 触摸屏中断 */
		Isr_Tc();

	/* 清中断 */
	SUBSRCPND |= (1 << INT_ADC_S) | (1 << INT_TC);
}

void adc_ts_int_init(void)
{
	SUBSRCPND |= (1 << INT_ADC_S) | (1 << INT_TC);

	/* 注册触摸屏中断 */
	register_irq(31, AdcTsIntHandle);

	/* 使能中断控制器 */	
	INTSUBMSK &= ~((1 << INT_ADC_S) | (1 << INT_TC));
}


void adc_ts_init(void)
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

void touchscreen_init(unsigned int irq)
{
	/* 设置触摸屏接口:寄存器 */
	adc_ts_init();

	/* 设置触摸屏中断 */
	adc_ts_int_init();

	/* 进入"等待中断模式" */
	enter_wait_pen_down();
}


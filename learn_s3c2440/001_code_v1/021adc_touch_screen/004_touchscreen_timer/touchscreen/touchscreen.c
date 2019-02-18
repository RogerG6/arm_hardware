#include "../s3c2440_soc.h"
#include "../interrupt.h"
#include "../my_printf.h"
#include "../uart.h"
//#include "../timer_init.h"


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

static volatile int g_ts_timer_enable = 0;

void enter_wait_pen_down(void)
{	
	ADCTSC = PEN_DOWN | XM_DISABLE | XP_DISABLE | YP_DISABLE | YM_ENABLE | PULLUP_ENABLE | WAIT_MODE;
}

void enter_wait_pen_up(void)
{	
	ADCTSC = PEN_UP | XM_DISABLE | XP_DISABLE | YP_DISABLE | YM_ENABLE | PULLUP_ENABLE | WAIT_MODE;
}

void enter_auto_measure_mode(void)
{
	ADCTSC = AUTO_PST | NO_OPR;
}

void Isr_Tc(void)
{	
//	printf("ADCDAT0 = 0x%08x\n\r", ADCDAT0);
	if (ADCDAT0 & (1 << 15))	/* pen up */
	{
	
		//puts("pen up ");
		
		//printf("x = 0x%x, y = 0x%x\n\r", ADCDAT0, ADCDAT1);  
		enter_wait_pen_down();
	}
	else						/* pen down */
	{
		puts("pen down ");

		/* 进入自动测量模式 */
		enter_auto_measure_mode();

		/* 启动ADC */
		ADCCON |= (1 << 0);
	}
}

static void ts_timer_enable(void)
{
	g_ts_timer_enable = 1;
}

static void ts_timer_disable(void)
{
	g_ts_timer_enable = 0;
}

static int get_ts_timer_status(void)
{
	return g_ts_timer_enable;
}


/* 每10ms被调用一次 */
void timer_touchscreen(void)
{
	if (!get_ts_timer_status())
		return ;
	if (ADCDAT0 & (1 << 15))	/* pen up */
	{
		ts_timer_disable();
		enter_wait_pen_down();
	}
	else						/* pen down */
	{
		/* 进入自动测量模式 */
		enter_auto_measure_mode();

		/* 启动ADC */
		ADCCON |= (1 << 0);
	}
}


void Isr_Adc(void)
{
	int x = ADCDAT0;
	int y = ADCDAT1;

	if (!(x & (1 << 15)))    /* 如果仍然按下, 才打印 */
	{
		x &= 0x3ff;
		y &= 0x3ff;
		printf("x = %d, y = %d\n\r", x, y);
		ts_timer_enable();
	}
	else
	{
		ts_timer_disable();
		enter_wait_pen_down();
	}
	
	enter_wait_pen_up();
}

void AdcTsIntHandle(unsigned int irq)
{

	//printf("SUBSRCPND = 0x%08x\n\r", SUBSRCPND);
	if (SUBSRCPND & (1 << INT_TC)) /* 触摸屏中断 */ 
		Isr_Tc();
#if 1		
	if (SUBSRCPND & (1 << INT_ADC_S)) /* ADC中断 */ 	
		Isr_Adc();
#endif
	/* 清中断 */
	SUBSRCPND |= (1 << INT_ADC_S) | (1 << INT_TC);
}

void adc_ts_int_init(void)
{
	 
	SUBSRCPND |= (1 << INT_ADC_S) | (1 << INT_TC);

	/* 注册触摸屏中断 */
	register_irq(AdcTsIntHandle, 31);

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

	ADCDLY = 60000;    /* 增加delay的时间，使电压稳定，输出的坐标更准确 */
}

void touchscreen_init(unsigned int irq)
{
	/* 设置触摸屏接口:寄存器 */
	adc_ts_init();

	/* 设置触摸屏中断 */
	adc_ts_int_init();

	/* 注册 timer  函数 */
	register_timer("touchscreen", timer_touchscreen);
	
	/* 进入"等待中断模式" */
	enter_wait_pen_down();
}


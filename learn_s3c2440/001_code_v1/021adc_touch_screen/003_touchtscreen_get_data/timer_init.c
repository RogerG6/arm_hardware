
# include "s3c2440_soc.h"
# include "interrupt.h"

void timer0_init(void)
{
	/* TCFG0, 设置timer input频率
	 * Timer input clock Frequency = PCLK / {prescaler value+1} / {divider value}
	 */
	TCFG0 = 99;
	/* TCFG1,  
	 *        MUX 0 设置为External TCLK0, Timer0 频率为0.25M*/
	 
	TCFG1 = 0;

	
	/* TCNTB0
     * TCMPB0   当TCNTB0 = TCMPB0，输出电平	
	 * TCNTO0	用于观察Timer0的值 */
	 TCNTB0 = 60000;
	
	/* TCON, 设置timer0为auto reload，manual update 并启动 */
	TCON |= (1 << 1);
	
	TCON &= ~(1 << 1);
	TCON |= ((1 << 0) | (1 << 3));
	register_irq(10, timer0_func);
	
}

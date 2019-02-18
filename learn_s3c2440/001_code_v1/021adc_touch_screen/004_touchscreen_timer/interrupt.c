/* SRCPND: 显示哪个中断产生了，0：清除中断源 
 * bit0--eint0
 * bit2--eint2
 * bit5--eint8_23
 */

/* INTMOD: 中断模式设置位，0 = IRQ mode 1 = FIQ mode 
 * bit0--eint0
 * bit2--eint2
 * bit5--eint8_23
 */

/* INTMSK: 中断屏蔽寄存器，0:cpu响应中断源，1：cpu屏蔽中断源 
 * bit0--eint0
 * bit2--eint2
 * bit5--eint8_23
 */

/* INTPND:  中断等待寄存器，显示当前优先级最高、正在发生的中断，需要清除相应的位, clear after SRCPND
 * 			0：清除， 1：等待被响应
 * bit0--eint0
 * bit2--eint2
 * bit5--eint8_23
 */
 
/* INTOFFSET: 显示哪个中断在INTPND中，在clear  SRCPND & INTPND后, 相应bit会自动清除 */


# include "interrupt.h"


void register_irq(irq_func pf, unsigned int irq)
{
	printf("irq = %d\n\r", irq);
	irq_arr[irq] = pf;         //注册中断处理函数
	INTMSK &= ~(1 << irq);     //使能中断控制器
}


/* 读EINTPEND分辨是哪个eint产生的中断
 * 清除中断时，写EINTPEND相应位 */
void key_irq_func(unsigned int irq)
{
	unsigned int val1 = EINTPEND;
	unsigned int val2 = GPFDAT;
	unsigned int val3 = GPGDAT;
	
	if (irq == 0)                 //eint0--d12
	{
		if (val2 & (1 << 0))
			GPFDAT |= (1 << 6);
		else
			GPFDAT &= ~(1 << 6);
	}
	else if (irq == 2)           //eint2--d11
	{
		if (val2 & (1 << 2))
			GPFDAT |= (1 << 5);
		else	
			GPFDAT &= ~(1 << 5);
	}
	else if (irq == 5)           
	{
		if (val1 & (1 << 11))    //eint11--d10
		{
			if (val3 & (1 << 3))
				GPFDAT |= (1 << 4);
			else	
				GPFDAT &= ~(1 << 4);
		}
		else if (val1 & (1 << 19))     //控制所有led on/off
		{
			if (val3 & (1 << 11))
			{
				GPFDAT |= (1 << 6);
				GPFDAT |= (1 << 5);
				GPFDAT |= (1 << 4);
			}
			else
			{
				GPFDAT &= ~(1 << 6);
				GPFDAT &= ~(1 << 5);
				GPFDAT &= ~(1 << 4);
			}
		}
	}
	EINTPEND = val1;
}





//按键中断处理函数
void handle_irq(void)
{
	unsigned int bit = INTOFFSET;

	
//	printf("bit = %d\n\r", bit);
	
	irq_arr[bit](bit);
	
	//清除中断，从源头开始清
	SRCPND |= (1 << bit);
	INTPND |= (1 << bit);
}






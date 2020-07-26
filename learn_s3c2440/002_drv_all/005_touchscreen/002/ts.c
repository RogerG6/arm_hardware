#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <asm/plat-s3c24xx/ts.h>

#include <asm/arch/regs-adc.h>
#include <asm/arch/regs-gpio.h>

struct ts_regs {
	unsigned long adccon;
	unsigned long adctsc;
	unsigned long adcdly;	
	unsigned long adcdat0;
	unsigned long adcdat1;		
	unsigned long adcupdn;
};

static struct input_dev * s3c_ts_dev;
static volatile struct ts_regs * ts_regs;

static void enter_wait_pen_down_mode(void)
{
	ts_regs->adctsc = 0xd3;
}

static void enter_wait_pen_up_mode(void)
{
	ts_regs->adctsc = 0x1d3;
}

static irqreturn_t pen_down_up_irq(int irq, void *dev_id)
{
	if (ts_regs->adcdat0 & (1<<15))
	{
		printk("pen up\n");
		enter_wait_pen_down_mode();
	}
	else
	{
		printk("pen down\n");
		enter_wait_pen_up_mode();
	}

	return IRQ_HANDLED;
}



static int ts_init(void)
{
	struct clk * clk;

	/* 1. 分配一个input_dev */
	s3c_ts_dev = input_allocate_device();
	if (!s3c_ts_dev)
		printk("Fail to allocate s3c_ts_dev");

	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, s3c_ts_dev->evbit);
	set_bit(EV_ABS, s3c_ts_dev->evbit);

	/* 2.2 能产生类中的哪些事件 */
	set_bit(BTN_TOUCH, s3c_ts_dev->keybit);

	input_set_abs_params(s3c_ts_dev, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_PRESSURE, 0, 1, 0, 0);

	/* 3. 注册 */
	input_register_device(s3c_ts_dev);

	/* 4. 硬件相关操作 */
	/* 4.1 使能CLKCON[5] */
	clk = clk_get(NULL, "adc");
	clk_enable(clk);

	/* 4.2 设置ts相关regs */
	ts_regs = ioremap(0x58000000, sizeof(struct ts_regs));

	/* PRSCEN [14]   : 1, A/D converter prescaler enable
 	 * PRSCVL [13:6] : 49, PCLK=50MHz, A/D converter freq = 50MHz/(49+1) = 1MHz
 	 * SEL_MUX[5:3]  : 0
 	 * STDBM  [2]    : 0 = Normal operation mode
 	 * READ_ START[1]: 0
 	 * ENABLE_START[0] : 0=no operation 在启动adc时再设置
	 */
	ts_regs->adccon = (1<<14) | (49<<6);

	request_irq(IRQ_TC, pen_down_up_irq, IRQF_SAMPLE_RANDOM, "ts_pen", NULL);

	enter_wait_pen_down_mode();
	
	
	return 0;
}

static void ts_exit(void)
{
	free_irq(IRQ_TC, NULL);
	iounmap(ts_regs);
	input_unregister_device(s3c_ts_dev);
	input_free_device(s3c_ts_dev);
}

module_init(ts_init);
module_exit(ts_exit);
MODULE_LICENSE("GPL");


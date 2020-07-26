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
static struct timer_list ts_timer;

static void enter_wait_pen_down_mode(void)
{
	ts_regs->adctsc = 0xd3;
}

static void enter_wait_pen_up_mode(void)
{
	ts_regs->adctsc = 0x1d3;
}

static void enter_measure_xy_mode(void)
{
	ts_regs->adctsc = (1<<3) | (1<<2);	
}

static void start_adc(void)
{
	ts_regs->adccon |= (1<<0);	
}

static irqreturn_t pen_down_up_irq(int irq, void *dev_id)
{
	if (ts_regs->adcdat0 & (1<<15))
	{
		//printk("pen up\n");
		enter_wait_pen_down_mode();
	}
	else
	{
		//printk("pen down\n");
		enter_measure_xy_mode();
		start_adc();
	}

	return IRQ_HANDLED;
}

/* 软件过滤，若相邻值偏差大于ERR_LIMIT，则返回0；否则返回1 */
static int filter(int x[], int y[], int cnt)
{
#define ERR_LIMIT 10
	int i;
	for (i = 1; i < cnt; i++)
	{
		if (((x[i] - x[i-1]) > ERR_LIMIT) || ((y[i] - y[i-1]) > ERR_LIMIT))
			return 0;
	}

	return 1;
}

static void s3c_ts_timer_function(unsigned long val)
{
	if (ts_regs->adcdat0 & (1<<15))
	{
		//已经松开
		enter_wait_pen_down_mode();
	}
	else
	{
		/* 按下，开始测量 */
		enter_measure_xy_mode();
		start_adc();
	}
}

static irqreturn_t ts_adc_irq(int irq, void *dev_id)
{
	static int cnt;
	int adcdat0, adcdat1;
	static int x[4], y[4];
	
	adcdat0 = ts_regs->adcdat0 & 0x3ff;
	adcdat1 = ts_regs->adcdat1 & 0x3ff;

	/* 优化措施2：如果ADC完成时, 发现触摸笔已经松开, 则丢弃此次结果 */
	if (ts_regs->adcdat0 & (1<<15))
	{
		/* 已经松开 */
		//printk("pen up\n");
		enter_wait_pen_down_mode();
	}
	else
	{
		/* 优化措施3：多次测量取平均 
		 * 优化措施4：用软件过滤，如果相邻值偏差较大则舍弃
		 */
		x[cnt] = adcdat0 & 0x3ff;
		y[cnt] = adcdat1 & 0x3ff;
		++cnt;
		
		if (cnt == 4)
		{
			if  (filter(x, y, cnt))
			{
				printk("x = %d, y = %d\n", (x[0]+x[1]+x[2]+x[3])/4, (y[1]+y[2]+y[3]+y[0])/4);
			}
			cnt = 0;
			enter_wait_pen_up_mode();
			
			/* 启动定时器处理长按/滑动的情况 */
			mod_timer(&ts_timer, jiffies + HZ/100);
		}
		else
		{
			enter_measure_xy_mode();
			start_adc();
		}
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
	request_irq(IRQ_ADC, ts_adc_irq, IRQF_SAMPLE_RANDOM, "adc", NULL);

	/* 优化错施1: 
	 * 设置ADCDLY为最大值, 这使得电压稳定后再发出IRQ_TC中断
	 */
	ts_regs->adcdly = 0xffff;

	/* 优化措施5：利用timer, 实现长按 */
	init_timer(&ts_timer);
	ts_timer.function = s3c_ts_timer_function;
	add_timer(&ts_timer);
	
	enter_wait_pen_down_mode();
	
	
	return 0;
}

static void ts_exit(void)
{
	del_timer(&ts_timer);
	free_irq(IRQ_ADC, NULL);
	free_irq(IRQ_TC, NULL);
	iounmap(ts_regs);
	input_unregister_device(s3c_ts_dev);
	input_free_device(s3c_ts_dev);
}

module_init(ts_init);
module_exit(ts_exit);
MODULE_LICENSE("GPL");


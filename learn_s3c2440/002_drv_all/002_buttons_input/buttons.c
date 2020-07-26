#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include <linux/gpio_keys.h>

#include <asm/gpio.h>

struct pin_desc {
	unsigned int irq;
	char * name;
	unsigned int pin;
	unsigned int keyval;
};

struct pin_desc pin[4] = {
	{IRQ_EINT0,  "S2", S3C2410_GPF0, KEY_L},
	{IRQ_EINT2,  "S3", S3C2410_GPF2, KEY_S},
	{IRQ_EINT11, "S4", S3C2410_GPG3, KEY_ENTER},
	{IRQ_EINT19, "S5", S3C2410_GPG11, KEY_LEFTSHIFT},
};

static struct input_dev * buttons_dev;
static struct pin_desc *irq_pd;
static struct timer_list buttons_timer;

static irqreturn_t buttons_irq(int irq, void *dev_id)
{
	/* 10ms后启动定时器 */
	irq_pd = (struct pin_desc *)dev_id;
	mod_timer(&buttons_timer, jiffies+HZ/100);
	return IRQ_RETVAL(IRQ_HANDLED);
}

static void buttons_timer_function(unsigned long data)
{
	struct pin_desc * pindesc = irq_pd;
	unsigned int pinval;

	if (!pindesc)
		return;
	
	pinval = s3c2410_gpio_getpin(pindesc->pin);

	if (pinval)
	{
		/* 松开：0 */
		input_event(buttons_dev, EV_KEY, pindesc->keyval, 0);
		input_sync(buttons_dev);
	}
	else
	{
		/* 按下：1 */
		input_event(buttons_dev, EV_KEY, pindesc->keyval, 1);
		input_sync(buttons_dev);
	}
}

static int buttons_init(void)
{
	int error, i;
	
	/* 1. 分配input_dev结构体 */
	buttons_dev = input_allocate_device();
	if (!buttons_dev)
		return -ENOMEM;

	/* 2. 设置 */
	/* 2.1 设置能产生哪类事件 */
	set_bit(EV_KEY, buttons_dev->evbit);
	set_bit(EV_REP, buttons_dev->evbit);

	/* 2.2 设置能产生类中的哪些事件 */
	set_bit(KEY_L, buttons_dev->keybit);
	set_bit(KEY_S, buttons_dev->keybit);
	set_bit(KEY_ENTER, buttons_dev->keybit);
	set_bit(KEY_LEFTSHIFT, buttons_dev->keybit);
	
	/* 3、注册 */
	error = input_register_device(buttons_dev);
	if (error) 
	{
		printk(KERN_ERR "Unable to register gpio-keys input device\n");
	}

	/* 4、硬件相关操作 */
	/* 4.1 定时器初始化 */
	init_timer(&buttons_timer);
	buttons_timer.function = buttons_timer_function;
	add_timer(&buttons_timer);	

	/* 4.2 注册按键中断 */
	for (i = 0; i < 4; i++)
	{
		request_irq(pin[i].irq,  buttons_irq, IRQT_BOTHEDGE, pin[i].name, &pin[i]);
	}
}

static void buttons_exit(void)
{
	int i;

	for(i = 0; i < 4; i++)
		free_irq(pin[i].irq, buttons_irq);

	del_timer(&buttons_timer);
	input_unregister_device(buttons_dev);
	input_free_device(buttons_dev);
}


module_init(buttons_init);
module_exit(buttons_exit);

MODULE_LICENSE("GPL");


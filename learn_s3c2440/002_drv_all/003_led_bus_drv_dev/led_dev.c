#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>

struct resource led_res[] = {
	/* 寄存器 */
	[0] = {
		.start = 0x56000050,
		.end   = 0x56000050 + 8 - 1,
		.flags  = IORESOURCE_MEM,
	},
	/* 哪个引脚 */
	[1] = {
        .start = 5,
        .end   = 5,
        .flags = IORESOURCE_IRQ,
	}

};

static void led_release(struct device * dev)
{}

struct platform_device led_dev = {
	.name		  = "myled",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(led_res),
	.resource	  = led_res,
	.dev		  = {
		.release =  led_release,
	},
};

static int led_init(void)
{
	platform_device_register(&led_dev);
	return 0;
}

static void led_exit(void)
{
	platform_device_unregister(&led_dev);
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");


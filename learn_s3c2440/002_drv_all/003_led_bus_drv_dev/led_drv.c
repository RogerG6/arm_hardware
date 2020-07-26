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
#include <asm/uaccess.h>
#include <asm/io.h>


static int pin;
static volatile unsigned long * gpfcon = NULL;
static volatile unsigned long * gpfdat = NULL;
static struct class *cls;
static int major;


static int led_open(struct platform_device * pdev)
{
	/* 配置GPF4,5,6为输出 */
	*gpfcon &= ~(0x3<<(pin*2));
	*gpfcon |= (0x1<<(pin*2));
	return 0;
}

static int led_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	/* 从usr space向kernel space传参 */
	copy_from_user(&val, buf, count); //	copy_to_user();

	if (val == 1)
	{
		// 点灯
		*gpfdat &= ~(1<<pin);
	}
	else
	{
		// 灭灯
		*gpfdat |= (1<<pin);
	}
	
	return 0;
}


static struct file_operations led_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   led_open,     
	.write	=	led_write,	   
};

static int led_probe(struct platform_device * pdev)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	/* 1 ioremap */
	/* 从PA到VA的映射 */
	gpfcon = ioremap(res->start, res->end - res->start + 1);
	gpfdat = gpfcon + 1;

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	pin = res->start;

	/* 2 注册字符设备驱动 */
	printk("led_probe, device found\n\r");
	
	major = register_chrdev(0, "myled", &led_fops);  //注册, 0：系统自动分配主设备号
	/* 创建一个类 */ 
	cls = class_create(THIS_MODULE, "myled");

	/* 在类下创建一个设备 */ 
	class_device_create(cls, NULL, MKDEV(major, 0), NULL, "led"); /* /dev/led */

	return 0;
}

static int led_remove(struct platform_device * pdev)
{
	/* 1 取消注册字符设备驱动 */
	printk("led_remove, device removed\n\r");
	
	class_device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, "myled");  

	/* 2 iounmap */
	iounmap(gpfcon);
}

struct platform_driver led_drv = {
	.probe		= led_probe,
	.remove		= __devexit_p(led_remove),
	.driver		= {
		.name	= "myled",
	}
};

static int led_drv_init(void)
{
	platform_driver_register(&led_drv);
	return 0;
}

static void led_drv_exit(void)
{
	platform_driver_unregister(&led_drv);
}

module_init(led_drv_init);
module_exit(led_drv_exit);

MODULE_LICENSE("GPL");


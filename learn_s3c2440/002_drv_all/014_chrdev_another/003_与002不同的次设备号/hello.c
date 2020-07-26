/* 
 * 创建了如下设备：
 * /dev/hello1	can open
 * /dev/hello2	can open
 * /dev/hello3	can't open	因为没有分配到region
 * /dev/hello4	can open
 */


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/poll.h>
#include <linux/cdev.h>


static int hello_open(struct inode *inode, struct file *file)
{
	printk("hello_open\n");
	return 0;
}

static int hello_2_open(struct inode *inode, struct file *file)
{
	printk("hello_2_open\n");
	return 0;
}

/* 1. 确定主设备号 */
static int major;

/* 2. 分配file_operations */
static struct file_operations hello_fops = {
    .owner   =   THIS_MODULE,  
    .open    =   hello_open,     
};

static struct file_operations hello_2_fops = {
    .owner   =   THIS_MODULE,  
    .open    =   hello_2_open,     
};

static struct cdev hello_dev;
static struct cdev hello_2_dev;
static dev_t dev_id;
static struct class *cls;

static int hello_init(void)
{
#if 0
	major = register_chrdev(0, "third_drv", &third_drv_fops);	/* 注册 */

	thirddrv_class = class_create(THIS_MODULE, "third_drv");
	thirddrv_class_dev = class_device_create(thirddrv_class, NULL, MKDEV(major, 0), NULL, "buttons");
#else
#define HELLO_CNT 2
	
	if (major) 	/* 有major */ 
	{
		dev_id = MKDEV(major, 1);
		register_chrdev_region(dev_id, HELLO_CNT, "hello");	/* [major,0]~[major,1]对应fops */
	} 
	else		/* 无major */
	{
		alloc_chrdev_region(&dev_id, 1, HELLO_CNT, "hello"); /* [major,0]~[major,1]对应fops */
		major = MAJOR(dev_id);
	}

	cdev_init(&hello_dev, &hello_fops);
	cdev_add(&hello_dev, dev_id, HELLO_CNT);

	
	dev_id = MKDEV(major, 4);
	register_chrdev_region(dev_id, 1, "hello2");
	cdev_init(&hello_2_dev, &hello_2_fops);
	cdev_add(&hello_2_dev, dev_id, 1);


	cls = class_create(THIS_MODULE, "hello");
	class_device_create(cls, NULL, MKDEV(major, 1), NULL, "hello1");
	class_device_create(cls, NULL, MKDEV(major, 2), NULL, "hello2");
	class_device_create(cls, NULL, MKDEV(major, 3), NULL, "hello3");
	class_device_create(cls, NULL, MKDEV(major, 4), NULL, "hello4");
#endif
	
	return 0;
}

static void hello_exit(void)
{
	class_device_destroy(cls, MKDEV(major, 1));
	class_device_destroy(cls, MKDEV(major, 2));
	class_device_destroy(cls, MKDEV(major, 3));
	class_device_destroy(cls, MKDEV(major, 4));
	class_destroy(cls);
	cdev_del(&hello_dev);
	unregister_chrdev_region(MKDEV(major, 1), HELLO_CNT);

	cdev_del(&hello_2_dev);
	unregister_chrdev_region(MKDEV(major, 4), 1);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");



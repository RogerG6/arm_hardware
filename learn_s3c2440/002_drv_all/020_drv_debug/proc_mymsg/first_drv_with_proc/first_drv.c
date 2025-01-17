#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>


static struct class *firstdrv_class;
static struct class_device	*firstdrv_class_dev;

extern int myprintk(const char *fmt, ...);


volatile unsigned long * gpfcon = NULL;
volatile unsigned long * gpfdat = NULL;

static int first_drv_open(struct inode *inode, struct file *file)
{
	//printk("first_drv_open\n");
	/* 配置GPF4,5,6为输出 */
	myprintk("first_drv_open\n");
	
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));
	*gpfcon |= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));
	return 0;
}


static ssize_t first_drv_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
	int val;

	myprintk("first_drv_write\n");
	

	/* 从usr space向kernel space传参 */
	copy_from_user(&val, buf, count); //	copy_to_user();

	if (val == 1)
	{
		// 点灯
		*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
	}
	else
	{
		// 灭灯
		*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	}
	
	return 0;
}

static struct file_operations first_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   first_drv_open,     
	.write	=	first_drv_write,	   
};


int major;
static int first_drv_init(void)
{
	myprintk("first_drv_init\n");
	

	major = register_chrdev(0, "first_drv", &first_drv_fops);  //注册, 0：系统自动分配主设备号

	/* 创建一个类 */ 
	firstdrv_class = class_create(THIS_MODULE, "firstdrv");

	/* 在类下创建一个设备 */ 
	firstdrv_class_dev = class_device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "xyz"); /* /dev/xyz */

	/* 从PA到VA的映射 */
	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;
	
	return 0;
}

static int first_drv_exit(void)
{
	myprintk("first_drv_exit\n");
	
	unregister_chrdev(major, "first_drv");  //取消注册
	
	class_device_unregister(firstdrv_class_dev);
	class_destroy(firstdrv_class);

	/* 取消映射 */
	iounmap(gpfcon);
	
	return 0;
}

module_init(first_drv_init);
module_exit(first_drv_exit);


MODULE_LICENSE("GPL");



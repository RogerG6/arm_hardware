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

static struct class *seconddrv_class;
static struct class_device	*seconddrv_class_dev;

volatile unsigned long * gpfcon = NULL;
volatile unsigned long * gpfdat = NULL;
volatile unsigned long * gpgcon = NULL;
volatile unsigned long * gpgdat = NULL;

static int second_drv_open(struct inode *inode, struct file *file)
{
	/* 配置gpf0, 2为输入引脚 */
	*gpfcon &= ~((3<<0) | (3<<4));
	
	/* 配置gpg3, 11为输入引脚 */
	*gpgcon &= ~((3<<6) | (3<<22));

	return 0;
}

static ssize_t second_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	unsigned int keyval[4];
	int regval;

	if (size != sizeof(keyval))
		return -EINVAL;
		
	/* 读gpf0, 2 */
	regval = *gpfdat;
	keyval[0] = (regval & (1<<0)) ? 1 : 0;
	keyval[1] = (regval & (1<<2)) ? 1 : 0;

	/* 读gpg3, 11 */
	regval = *gpgdat;
	keyval[2] = (regval & (1<<3)) ? 1 : 0;
	keyval[3] = (regval & (1<<11)) ? 1 : 0;

	/* kernel中获取的数据向user传递 */
	copy_to_user(buf, keyval, sizeof(keyval));	

	return  sizeof(keyval);
}

static struct file_operations second_drv_fops = {
    .owner  =   THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open   =   second_drv_open,     
	.read	=	second_drv_read,	   
};

int major;
/* 驱动入口函数 */
static int second_drv_init(void)
{
	major = register_chrdev(0, "second_drv", &second_drv_fops);	/* 注册 */

	seconddrv_class = class_create(THIS_MODULE, "second_drv");
	seconddrv_class_dev = class_device_create(seconddrv_class, NULL, MKDEV(major, 0), NULL, "buttons");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;
	return 0;
}

static int second_drv_exit(void)
{
	unregister_chrdev(major, "buttons");	/* 卸载 */ 

	class_device_unregister(seconddrv_class_dev);
	class_destroy(seconddrv_class);

	iounmap(gpfcon);
	iounmap(gpgcon);
	return 0;
}

module_init(second_drv_init);
module_exit(second_drv_exit);

MODULE_LICENSE("GPL");




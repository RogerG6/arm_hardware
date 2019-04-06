#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/poll.h>

static struct class *sixthdrv_class;
static struct class_device	*sixthdrv_class_dev;

volatile unsigned long * gpfcon = NULL;
volatile unsigned long * gpfdat = NULL;
volatile unsigned long * gpgcon = NULL;
volatile unsigned long * gpgdat = NULL;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

/* 中断事件标志, 中断服务程序将它置1，sixth_drv_read将它清0 */
static volatile int ev_press = 0;

static struct fasync_struct *button_async;

struct pin_desc {
	unsigned int pin;
	unsigned int keyval;
};

/* 按下：keyval = 0x01
 * 松开：keyval = 0x81
 */
struct pin_desc pin[4] = {
	{S3C2410_GPF0, 0x01},
	{S3C2410_GPF2, 0x02},
	{S3C2410_GPG3, 0x03},
	{S3C2410_GPG11, 0x04},
};

static unsigned char keyval;

static irqreturn_t buttons_irq(int irq, void * dev_id)
{
	struct pin_desc * p_pin_desc = (struct pin_desc *)dev_id;
	unsigned int pinval = s3c2410_gpio_getpin(p_pin_desc->pin);

	if (pinval)
		keyval = 0x80 | p_pin_desc->keyval;	/* 松开 */
	else
		keyval = p_pin_desc->keyval;		/* 按下 */

    ev_press = 1;                  /* 表示中断发生了 */
    wake_up_interruptible(&button_waitq);   /* 唤醒休眠的进程 */
    
	kill_fasync (&button_async, SIGIO, POLL_IN);  /* 向usr发信号 */

	return IRQ_RETVAL(IRQ_HANDLED);
}

/* 定义canopen原子量，并初始化为1 */
atomic_t canopen = ATOMIC_INIT(1);

static DECLARE_MUTEX(button_lock);     //定义互斥锁

static int sixth_drv_open(struct inode *inode, struct file *file)
{
#if 0
	/* 1. 原子操作 */
	/* caopen自减1后与0比较，相等返回true, 否则返回false */
	if (!atomic_dec_and_test(&canopen))	
	{
		atomic_inc(&canopen);   /* canopen自加1 */
		return -EBUSY;
	}
#endif

	/* 3. 阻塞
	 * 判断是否是阻塞
	 */
	 if (file->f_flags & O_NONBLOCK)
	 {
		if (down_trylock(&button_lock))		/* 非阻塞 */
			return -EBUSY;
	 }
	 else
	 {

		/* 2. 信号量 */
		/* 获取信号量 */
		down(&button_lock);					/* 阻塞 */
	 }
	
	/* 配置gpf0, 2为中断引脚 */
	/* 配置gpg3, 11为中断引脚 */
	request_irq(IRQ_EINT0,  buttons_irq, IRQT_BOTHEDGE, "S2", &pin[0]);
	request_irq(IRQ_EINT2,  buttons_irq, IRQT_BOTHEDGE, "S3", &pin[1]);
	request_irq(IRQ_EINT11, buttons_irq, IRQT_BOTHEDGE, "S4", &pin[2]);		
	request_irq(IRQ_EINT19, buttons_irq, IRQT_BOTHEDGE, "S5", &pin[3]);

	return 0;
}

static ssize_t sixth_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{

	if (size != 1)
		return -EINVAL;

	 if (file->f_flags & O_NONBLOCK)
	 {
		if (!ev_press)		/* 非阻塞 */
			return -EAGAIN;
	 }
	 else					/* 阻塞 */
	 {
		/* 如果没有按键动作(ev_press = 0), 休眠 */
		wait_event_interruptible(button_waitq, ev_press);
	 }

	/* kernel中获取的数据向user传递 */
	copy_to_user(buf, &keyval, 1);	
	ev_press = 0;

	return  1;
}

static int sixth_drv_close(struct inode * inode, struct file * file)
{
	//atomic_inc(&canopen);

	free_irq(IRQ_EINT0, &pin[0]);
	free_irq(IRQ_EINT2, &pin[1]);
	free_irq(IRQ_EINT11, &pin[2]);
	free_irq(IRQ_EINT19, &pin[3]);

	up(&button_lock);

	return 0;
}

static unsigned int sixth_drv_poll(struct file * file, struct poll_table_struct * wait)
{
	unsigned int mask = 0;
	poll_wait(file, &button_waitq, wait); // 不会立即休眠

	/* mask是驱动向poll传递的参数
	 * ev_press = 1, 有数据向usr传递，mask != 0, 唤醒
	 * ev_press = 0, 无数据向usr传递，mask  = 0, 休眠
	 */
	if (ev_press)
		mask |= POLLIN | POLLRDNORM;

	return mask;
}

static int sixth_drv_fasync (int fd, struct file *filp, int on)
{
	printk("driver: sixth_drv_fasync\n");
	return fasync_helper (fd, filp, on, &button_async);
}

static struct file_operations sixth_drv_fops = {
    .owner   =  THIS_MODULE,    /* 这是一个宏，推向编译模块时自动创建的__this_module变量 */
    .open    =  sixth_drv_open,     
	.read	 =	sixth_drv_read,
	.release =  sixth_drv_close,
	.poll    =  sixth_drv_poll,
	.fasync  =  sixth_drv_fasync,
};

int major;
/* 驱动入口函数 */
static int sixth_drv_init(void)
{
	major = register_chrdev(0, "sixth_drv", &sixth_drv_fops);	/* 注册 */

	sixthdrv_class = class_create(THIS_MODULE, "sixth_drv");
	sixthdrv_class_dev = class_device_create(sixthdrv_class, NULL, MKDEV(major, 0), NULL, "buttons");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
	gpfdat = gpfcon + 1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060, 16);
	gpgdat = gpgcon + 1;
	return 0;
}

static int sixth_drv_exit(void)
{
	unregister_chrdev(major, "buttons");	/* 卸载 */ 

	class_device_unregister(sixthdrv_class_dev);
	class_destroy(sixthdrv_class);

	iounmap(gpfcon);
	iounmap(gpgcon);
	return 0;
}

module_init(sixth_drv_init);
module_exit(sixth_drv_exit);

MODULE_LICENSE("GPL");




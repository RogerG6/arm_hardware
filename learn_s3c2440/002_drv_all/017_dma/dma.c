/*
 * 通过dma，将内存中source的512k拷贝到dest
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
#include <linux/dma-mapping.h>

#define BUF_SIZE (512*1024)
#define MEM_CPY_DMA    1
#define MEM_CPY_NO_DMA 0
#define CH0 0x4B000000		/* dma channels */
#define CH1 0x4B000040
#define CH2 0x4B000080
#define CH3 0x4B0000C0

static int major = 0;
static struct class *cls;

static u32 src_phys;
static char * src;

static u32 dst_phys;
static char * dst;
static DECLARE_WAIT_QUEUE_HEAD(dma_waitq);

/* 中断事件标志, 中断服务程序将它置1，ioctl将它清0 */
static volatile int ev_dma = 0;

struct dma_regs {
	unsigned long disrc    ;
	unsigned long disrcc   ;
	unsigned long didst    ;
	unsigned long didstc   ;
	unsigned long dcon     ;
	unsigned long dstat    ;
	unsigned long dcsrc    ;
	unsigned long dcdst    ;
	unsigned long dmasktrig;
};

static volatile struct dma_regs * dma_regs;

static int s3c_dma_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int i;

	memset(src, 0x11, BUF_SIZE);
	memset(dst, 0xaa, BUF_SIZE);
	
	switch(cmd)
	{
		case MEM_CPY_DMA:
		{
			ev_dma = 0;
			
			/* dma硬件相关设置 */
			dma_regs->disrc = src_phys;			/* SRC物理地址 */	
			dma_regs->disrcc = (0<<1) | (0<<0);	/* AHB, INCRESMENT */
			dma_regs->didst = dst_phys;			/* DST物理地址 */	
			dma_regs->didstc = (0<<2) | (0<<1) | (0<<0); /* 中断，AHB, INCRESMENT */

			/* 使能中断，单个传输，软件触发 */
			dma_regs->dcon = (1<<30)|(1<<29)|(0<<28)|(1<<27)|(0<<23)|(0<<20)|(BUF_SIZE<<0);

			/* 启动DMA */
			dma_regs->dmasktrig = (1<<1) | (1<<0);

			/* 如何知道DMA什么时候完成? */
			/* 休眠 */
			wait_event_interruptible(dma_waitq, ev_dma);

			if (memcmp(src, dst, BUF_SIZE) == 0)
			{
				printk("MEM_CPY_DMA OK\n");
			}
			else
			{
				printk("MEM_CPY_DMA ERROR\n");
			}

			break;
		}
		
		case MEM_CPY_NO_DMA:
		{
			for (i = 0; i < BUF_SIZE; i++)
				dst[i] = src[i];
				
			if (memcmp(src, dst, BUF_SIZE) == 0)
			{
				printk("MEM_CPY_NO_DMA OK\n");
			}
			else
			{
				printk("MEM_CPY_NO_DMA ERROR\n");
			}
			break;
		}
	}
	return 0;
}

static struct file_operations s3c_dma_fops = {
	.owner = THIS_MODULE,
	.ioctl = s3c_dma_ioctl,
};

static irqreturn_t s3c_dma_irq(int irq, void *devid)
{
	/* 唤醒 */
	ev_dma = 1;
    wake_up_interruptible(&dma_waitq);   /* 唤醒休眠的进程 */
	return IRQ_HANDLED;
}

static int s3c_dma_init(void)
{
	if (request_irq(IRQ_DMA3, s3c_dma_irq, 0, "s3c_dma", 1))
	{
		printk("can't request_irq for DMA\n");
		return -EBUSY;
	}

	/* 分配SRC, DST对应的缓冲区, 用该函数分配的是物理地址连续的空间 */
	src = dma_alloc_writecombine(NULL, BUF_SIZE, &src_phys, GFP_KERNEL);
	if (src == NULL)
	{
		printk("source alloc failed\n");
		return -ENOMEM;
	}
	
	dst = dma_alloc_writecombine(NULL, BUF_SIZE, &dst_phys, GFP_KERNEL);
	if (src == NULL)
	{
		printk("destination alloc failed\n");
		dma_free_writecombine(NULL, BUF_SIZE, src, src_phys);
		return -ENOMEM;
	}
	

	/* 注册字符设备，创建类和类下的设备节点 */
	major = register_chrdev(0, "s3c_dma", &s3c_dma_fops);
	cls = class_create(THIS_MODULE, "s3c_dma");
	class_device_create(cls, NULL, MKDEV(major, 0), NULL, "dma");	/* /dev/dma */

	dma_regs = ioremap(CH3, sizeof(struct dma_regs));
	
	return 0;
}

static void s3c_dma_exit(void)
{
	class_device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, "s3c_dma");
	dma_free_writecombine(NULL, BUF_SIZE, src, src_phys);
	dma_free_writecombine(NULL, BUF_SIZE, dst, dst_phys);
	free_irq(IRQ_DMA3, 1);
}

module_init(s3c_dma_init);
module_exit(s3c_dma_exit);
MODULE_LICENSE("GPL");



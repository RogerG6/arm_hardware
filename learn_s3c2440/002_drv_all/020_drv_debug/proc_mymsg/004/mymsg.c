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
#include <linux/proc_fs.h>

#define BUF_LEN 1024
struct proc_dir_entry *myentry;
static char mylog_buf[BUF_LEN];
static char tmp_buf[BUF_LEN];
static int mylog_r = 0;
static int mylog_w = 0;
static DECLARE_WAIT_QUEUE_HEAD(mymsg_waitq);


static int is_mylog_empty(void)
{
	return (mylog_r == mylog_w);
}

static int is_mylog_full(void)
{
	return (((mylog_w + 1) % BUF_LEN) == mylog_r);
}

static int mylog_read(char *p)
{
	if (is_mylog_empty())
	{
		return 0;
	}
	
	*p = mylog_buf[mylog_r];
	mylog_r = (mylog_r + 1) % BUF_LEN;
	
	return 1;
}

static void mylog_write(char c)
{
	if (is_mylog_full())
	{
		/* 丢弃一个数据 */
		mylog_r = (mylog_r + 1) % BUF_LEN;
	}
	mylog_buf[mylog_w] = c;
	mylog_w = (mylog_w + 1) % BUF_LEN;	

	/* 唤醒等待数据的进程 */	
    wake_up_interruptible(&mymsg_waitq);   /* 唤醒休眠的进程 */	
}

int myprintk(const char *fmt, ...)
{
	va_list args;
	int i;
	int j;

	va_start(args, fmt);
	i=vsnprintf(tmp_buf, INT_MAX, fmt, args);
	va_end(args);

	for (j = 0; j < i; j++)
		mylog_write(tmp_buf[j]);
		
	return i;
}


static ssize_t mymsg_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	int error;
	char c;
	int i = 0;
	
	if ((file->f_flags & O_NONBLOCK) && is_mylog_empty())
		return -EAGAIN;

	error = wait_event_interruptible(mymsg_waitq, !is_mylog_empty());

	while (!error && (mylog_read(&c)) && i < count) {
		error = __put_user(c, buf);
		buf++;
		i++;
	}
	
	if (!error)
		error = i;
	
	return error;
}

const struct file_operations proc_mymsg_operations = {
	.read		= mymsg_read,
};

static int mymsg_init(void)
{
	myentry = create_proc_entry("mymsg", S_IRUSR, &proc_root);
	if (myentry)
		myentry->proc_fops = &proc_mymsg_operations;
	return 0;
}

static void mymsg_exit(void)
{
	remove_proc_entry("mymsg", &proc_root);
}

module_init(mymsg_init);
module_exit(mymsg_exit);

EXPORT_SYMBOL(myprintk);

MODULE_LICENSE("GPL");


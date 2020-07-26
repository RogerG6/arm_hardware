#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <asm/rtc.h>

static struct i2c_driver at24cxx_driver;
static unsigned short ignore[] = { I2C_CLIENT_END };
static unsigned short normal_addr[] = { 0x50, I2C_CLIENT_END };

static unsigned short force_addr[] = {ANY_I2C_BUS, 0x60, I2C_CLIENT_END};
static unsigned short * forces[] = {force_addr, NULL};

static struct i2c_client_address_data addr_data = {
	.normal_i2c	= normal_addr,
	.probe		= ignore,
	.ignore		= ignore,
	//.forces     = forces,
};

static struct i2c_client *at24cxx_client;	/* 用户，代表at24cxx这个设备 */

/* 字符设备 */
static int major;
static struct class * cls;

static ssize_t at24cxx_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	return 0;
}
static ssize_t at24cxx_write(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = at24cxx_read,
	.write = at24cxx_write,
};

static int at24cxx_detect(struct i2c_adapter *adap, int addr, int kind)
{

	printk("at24cxx_detect\n");

	at24cxx_client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);

	/* 设置client */
	at24cxx_client->addr = addr;
	at24cxx_client->adapter = adap;
	at24cxx_client->driver = &at24cxx_driver;
	strcpy(at24cxx_client->name, "at24cxx");

	i2c_attach_client(at24cxx_client);

	major = register_chrdev(0, "at24cxx", &fops);
	cls = class_create(THIS_MODULE, "at24cxx");
	class_device_create(cls, NULL, MKDEV(major, 0), NULL, "at24cxx"); /* /dev/at24cxx */
	
	
	return 0;
}


static int at24cxx_attach(struct i2c_adapter *adapter)
{
	return i2c_probe(adapter, &addr_data, at24cxx_detect);
}

static int at24cxx_detach(struct i2c_client *client)
{
	printk("at24cxx_detach\n");

	class_device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	unregister_chrdev(major, "at24cxx");
	
	i2c_detach_client(at24cxx_client);
	kfree(i2c_get_clientdata(client));
	
	return 0;
}

/* 分配一个i2c_driver */
static struct i2c_driver at24cxx_driver = {
	.driver = {
		.name	= "at24cxx",
	},
	.attach_adapter	= at24cxx_attach,
	.detach_client	= at24cxx_detach,
};

/* 设置 */

static int at24cxx_init(void)
{
	i2c_add_driver(&at24cxx_driver);
	return 0;
}

static void at24cxx_exit(void)
{
	i2c_del_driver(&at24cxx_driver);
}

module_init(at24cxx_init);
module_exit(at24cxx_exit);
MODULE_LICENSE("GPL");



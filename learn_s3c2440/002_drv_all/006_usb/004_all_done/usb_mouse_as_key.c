/*
 * 参考/drviers/hid/usbhid/usbmouse.c
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static 	struct input_dev *uk_dev;	/* usb key */
static int len;
static char * usb_buf;
static dma_addr_t usb_buf_phys;
static struct urb * uk_urb;

static struct usb_device_id usb_mouse_as_key_id_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};

static void usb_mouse_as_key_irq(struct urb* urb)
{
	static unsigned char pre_val[7];
	int i;
#if 0
	int i;
	static int cnt = 0;
	printk("data cnt %d: ", ++cnt);
	for (i = 0; i < len; i++)
	{
		printk("%02x ", usb_buf[i]);
	}
	printk("\n");
#endif
	/* mouse数据含义：
	 * data[1]: bit[0]-左键
	 *		    bit[1]-右键
	 * data[5]: 中键
	 */
	if ((pre_val[1] & (1<<0)) != (usb_buf[1] & (1<<0)))
	{
		/* 左键发生了变化 */
		input_event(uk_dev, EV_KEY, KEY_L, (usb_buf[1] & (1<<0)) ? 1 : 0);
		input_sync(uk_dev);
	}

	if ((pre_val[1] & (1<<1)) != (usb_buf[1] & (1<<1)))
	{
		/* 右键发生了变化 */
		input_event(uk_dev, EV_KEY, KEY_S, (usb_buf[1] & (1<<1)) ? 1 : 0);
		input_sync(uk_dev);
	}
#if 1
	if (pre_val[5] != usb_buf[5])
	{
		/* 中键发生了变化 */
		input_event(uk_dev, EV_KEY, KEY_ENTER, 1);
		input_sync(uk_dev);
	}
#endif

	for (i = 0; i < 7; i++)
	{
		pre_val[i] = usb_buf[i];
	}

	/* 重新提交urb */
	usb_submit_urb(uk_urb, GFP_KERNEL);
}

static int usb_mouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_device *dev = interface_to_usbdev(intf);
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int pipe;

	interface = intf->cur_altsetting;
	endpoint = &interface->endpoint[0].desc;	

	/* 1. 分配input_dev */
	uk_dev = input_allocate_device();

	/* 2. 设置 */
	set_bit(EV_KEY, uk_dev->evbit);
	set_bit(EV_REP, uk_dev->evbit);

	set_bit(KEY_L, uk_dev->keybit);
	set_bit(KEY_S, uk_dev->keybit);
	set_bit(KEY_ENTER, uk_dev->keybit);

	/* 3. 注册 */
	input_register_device(uk_dev);

	/* 4. 硬件相关操作 */
	/* 数据传输3要素：源、目的、长度 */
	/* 4.1 源：USB设备的某个端点 */
	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	
	/* 4.2 长度 */
	len = endpoint->wMaxPacketSize;
	
	/* 4.3 目的 */
	usb_buf = usb_buffer_alloc(dev, len, GFP_ATOMIC, &usb_buf_phys);

	/* 4.4 使用3要素 */
	/* 分配一个usb request block */
	uk_urb = usb_alloc_urb(0, GFP_KERNEL);
	
	/* 使用"3要素设置urb" */
	usb_fill_int_urb(uk_urb, dev, pipe, usb_buf, len,
			 usb_mouse_as_key_irq, NULL, endpoint->bInterval);
	uk_urb->transfer_dma = usb_buf_phys;
	uk_urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	/* 使用urb */
	usb_submit_urb(uk_urb, GFP_KERNEL);

	return 0;
}

static void usb_mouse_as_key_disconnect(struct usb_interface *intf)
{
	struct usb_device *dev = interface_to_usbdev(intf);

	//printk("usb_mouse_as_key_disconnect\n");
	usb_kill_urb(uk_urb);
	usb_free_urb(uk_urb);
	usb_buffer_free(dev, len, usb_buf, usb_buf_phys);
	input_unregister_device(uk_dev);
	input_free_device(uk_dev);
}

static struct usb_driver usb_mouse_as_key_driver = {
	.name		= "usb_mouse_as_key",
	.probe		= usb_mouse_as_key_probe,
	.disconnect	= usb_mouse_as_key_disconnect,
	.id_table	= usb_mouse_as_key_id_table,
};

static int usb_mouse_as_key_init(void)
{
	usb_register(&usb_mouse_as_key_driver);
	return 0;
}

static void usb_mouse_as_key_exit(void)
{
	usb_deregister(&usb_mouse_as_key_driver);
}


module_init(usb_mouse_as_key_init);
module_exit(usb_mouse_as_key_exit);

MODULE_LICENSE("GPL");


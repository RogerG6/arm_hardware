

static struct input_dev * s3c_ts_dev;

static int ts_init(void)
{
	/* 1. 分配一个input_dev */
	s3c_ts_dev = input_allocate_device();
	if (!s3c_ts_dev)
		printk("Fail to allocate s3c_ts_dev");

	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, s3c_ts_dev->evbit);
	set_bit(EV_ABS, s3c_ts_dev->evbit);

	/* 2.2 能产生类中的哪些事件 */
	set_bit(BTN_TOUCH, s3c_ts_dev->keybit);

	input_set_abs_params(s3c_ts_dev, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(s3c_ts_dev, ABS_PRESSURE, 0, 1, 0, 0);

	/* 3. 注册 */
	input_register_device(s3c_ts_dev);

	/* 4. 硬件相关操作 */
	return 0;
}

static void ts_exit(void)
{

}

module_init(ts_init);
module_exit(ts_exit);
MODULE_LICENSE("GPL");


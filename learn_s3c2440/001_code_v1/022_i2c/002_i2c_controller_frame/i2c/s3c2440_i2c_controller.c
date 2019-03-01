# include "i2c_controller.h"
# include "../s3c2440_soc.h"



void s3c2440_irq_func(unsigned int irq)
{
	/* 每发送完一个数据，就会产生一次中断        	*/

	/* 每次发送，第一个中断是“已发送从设备地址” */
}

void s3c2440_i2c_con_init(void)
{
	/* 设置时钟
	 * [7] : ACK, 0: Disable, 1: Enable
	 * [6] : Tx clock source, 0: IICCLK = fPCLK /16, 1: IICCLK = fPCLK /512
	 * [5] : Tx/Rx Interrupt, 0: Disable, 1: Enable
	 * [4] : Interrupt pending flag, 1 = Interrupt is pending, 退出中断此位清0
	 * [3:0] : Transmit clock value, Tx clock = IICCLK/(IICCON[3:0]+1)
	 * 										  = 50MHz / 16 /(IICCON[3:0]+1) = 100kHz
	 */
	IICCON = (31 << 0) | (0 << 6) | (1 << 5);

	/* 注册中断 */
	register_irq(s3c2440_irq_func, 27);
	
}

int do_i2c_tranfer(p_i2c_msg msgs)
{
	/* 配置Master Tx mode 
	 * [7:6] : Mode selection
	 * [5]   : Busy signal status / START STOP condition
	 * [4]   : IIC-bus data output enable/disable bit.
	 * [3]   : Arbitration status flag
	 * [2]   : Address-as-slave status flag
	 * [1]   : Address zero status flag
	 * [0]   : Last-received bit status flag
	 */
	IICSTAT = (3 << 6);

	/* 写从设备地址到IICDS */
	IICADD = (msgs->addr << 1);

	/* IICSTAT = 0xf0, start信号，数据被发送出去 */
	IICSTAT =0xf0;

	/* 调用中断处理函数 */
	while (msgs->cnt_tranfer != msgs->len);

	/* IICSTAT = 0xd0, P信号 */
	IICSTAT = 0xd0;
}

int do_i2c_receive(p_i2c_msg msgs)
{
	/* 配置Master Tx mode 
	 * [7:6] : Mode selection
	 * [5]   : Busy signal status / START STOP condition
	 * [4]   : IIC-bus data output enable/disable bit.
	 * [3]   : Arbitration status flag
	 * [2]   : Address-as-slave status flag
	 * [1]   : Address zero status flag
	 * [0]   : Last-received bit status flag
	 */
	IICSTAT = (2 << 6);

	/* 写从设备地址到IICDS */
	IICADD = (msgs->addr << 1) | (1 << 0);

	/* IICSTAT = 0xB0, start信号，接收数据 */
	IICSTAT =0xB0;

	/* 调用中断处理函数 */
	while (msgs->cnt_tranfer != msgs->len);

	/* IICSTAT = 0x90, P信号 */
	IICSTAT = 0x90;
}

void s3c2440_i2c_tranfer(p_i2c_msg msgs, int num)
{
	if (!msgs->flags)		/* 读操作 */
		do_i2c_tranfer(msgs);
	else					/* 写操作 */
		do_i2c_receive(msgs);
}


/* 实现i2c_controller
		.init
		.master_tranfer
		.name
 */
static i2c_controller s3c2440_i2c_controller = {
	.init = s3c2440_i2c_con_init,
	.master_tranfer = s3c2440_i2c_tranfer,
	.name = "s3c2440",
};


/* 将s3c2440_i2c_controller添加到i2c_controllers中 */
void s3c2440_i2c_add(void)
{
	register_i2c_controller(s3c2440_i2c_controller);
}

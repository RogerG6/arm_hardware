# include "i2c_controller.h"
# include "../s3c2440_soc.h"

static p_i2c_msg p_g_msg;

int isLastData(void)
{
	if (p_g_msg->cnt_tranfer == p_g_msg->len - 1)
		return 1;
	else 
		return 0;
}

void resume_iic_with_ack(void)
{
	unsigned int iiccon = IICCON;
	iiccon |= (1<<7);
	iiccon &= ~(1<<4);
	IICCON = iiccon;
}

void resume_iic_without_ack(void)
{
	unsigned int iiccon = IICCON;
	iiccon &= ~((1<<4)|(1<<7));
	IICCON = iiccon;
}


void s3c2440_irq_func(unsigned int irq)
{
	unsigned int iicstat = IICSTAT;
	int index;

	p_g_msg->cnt_tranfer++;
	printf("In irq func : p_g_msg->cnt_tranfer = %d\n\r", p_g_msg->cnt_tranfer);

	/* 每发送完一个数据，就会产生一次中断        	*/
	/* 每次发送，第一个中断是“已发送从设备地址” */

	if (p_g_msg->flags == 0)	/* write */
	{
		
		/* 如果是第1次中断，则判断ACK信号
		 * 有ACK：有从设备，写数据到IICDS
		 * 无ACK：无从设备，停止传输
		 */
		 puts("000\n\r");
		 if (p_g_msg->cnt_tranfer == 0)	/* 第1次中断 */
		 {
		 	if (iicstat & (1 << 0))	/* no ACK */
		 	{
				IICSTAT = 0xd0;
				IICCON &= ~(1 << 4);
				delay(1000);
				p_g_msg->err = -1;
				puts("error in Tx\n\r");
				return;
		 	}
		 }
		 /* have ACK */
		if (p_g_msg->cnt_tranfer < p_g_msg->len)   /* 未写完 */
		{
			IICDS = p_g_msg->buf[p_g_msg->cnt_tranfer];
			//putchar(IICDS);
			IICCON &= ~(1 << 4);
			puts("001\n\r");
		} 
		else
		{
			IICSTAT = 0xd0;
			IICCON &= ~(1 << 4);
			delay(1000);
		}
		puts("002\n\r"); 
	}
	else						/* read */
	{
 		/* 如果是第1次中断，则判断ACK信号
		 * 有ACK：有从设备，返回，此时IICDS中无数据，第2次才能读取
		 * 无ACK：无从设备，停止接收
		 */
		 if (p_g_msg->cnt_tranfer == 0)	/* 第1次中断 */
		 {
		 	if (iicstat & (1 << 0))	/* no ACK */
		 	{
				IICSTAT = 0x90;
				IICCON &= ~(1 << 4);
				delay(1000);
				p_g_msg->err = -1;
				puts("error in Rx\n\r");				
				return;
		 	}
		 	else					/* have ACK */
		 	{
				/* 如果是最后一个数据，启动传输时设备为no ack */
				if (isLastData())
					resume_iic_without_ack();
				else
					resume_iic_with_ack();
		 		return;
		 	}
		 }
		 else						 
		 {
		 	index = p_g_msg->cnt_tranfer - 1;
			if (p_g_msg->cnt_tranfer < p_g_msg->len)   /* 未读完 */
			{
				p_g_msg->buf[index] = IICDS;
				putchar(IICDS);

				/* 如果是最后一个数据，启动传输时设备为no ack */
				if (isLastData())
					resume_iic_without_ack();
				else
					resume_iic_with_ack();
				
			}
			else
			{
				IICSTAT = 0x90;
				IICCON &= ~(1 << 4);
				delay(1000);
			}
		 }
	}
	puts("003\n\r");
}

void s3c2440_i2c_con_init(void)
{
	/* 配置引脚用于IIC */
	GPECON &= ~((3 << 28) | (3 << 30));
	GPECON |= ((2 << 28) | (2 << 30));
	
	/* 设置时   钟
	 * [7] : ACK, 0: Disable, 1: Enable
	 * [6] : Tx clock source, 0: IICCLK = fPCLK /16, 1: IICCLK = fPCLK /512
	 * [5] : Tx/Rx Interrupt, 0: Disable, 1: Enable
	 * [4] : Interrupt pending flag, 1 = Interrupt is pending, 退出中断此位清0
	 * [3:0] : Transmit clock value, Tx clock = IICCLK/(IICCON[3:0]+1)
	 * 										  = 50MHz / 16 /(IICCON[3:0]+1) = 100kHz
	 */
	IICCON = (1 << 7) | (0 << 6) | (1 << 5) | (30 << 0);

	/* 注册中断 */
	register_irq(s3c2440_irq_func, 27);
	
}

int do_i2c_tranfer(p_i2c_msg msgs)
{
	msgs->cnt_tranfer = -1;
	msgs->err = 0;
	p_g_msg = msgs;
	
	/* 配置Master Tx mode */
	IICCON |= (1<<7); /* TX mode, 在ACK周期释放SDA */
	IICSTAT |= (1 << 4);
	
	/* [7:6] : Mode selection
	 * [5]   : Busy signal status / START STOP condition
	 * [4]   : IIC-bus data output enable/disable bit.
	 * [3]   : Arbitration status flag
	 * [2]   : Address-as-slave status flag
	 * [1]   : Address zero status flag
	 * [0]   : Last-received bit status flag
	 */
	//IICSTAT = (3 << 6);

	/* 写从设备地址到IICDS */
	IICDS = (msgs->addr << 1);

	/* IICSTAT = 0xf0, start信号，数据被发送出去 */
	IICSTAT =0xf0;

	/* 调用中断处理函数 */
	while (!msgs->err && (msgs->cnt_tranfer != msgs->len))
		puts("loop");
	
	if (p_g_msg->err)
		return -1;
	return 0;
}

int do_i2c_receive(p_i2c_msg msgs)
{
 	msgs->cnt_tranfer = -1;
	msgs->err = 0;
	msgs->len++;
	p_g_msg = msgs;
	
	/* 配置Master Tx mode */ 
	IICCON |= (1<<7); /* RX mode, 在ACK周期回应 */
	IICSTAT |= (1 << 4);
	
	/* [7:6] : Mode selection
	 * [5]   : Busy signal status / START STOP condition
	 * [4]   : IIC-bus data output enable/disable bit.
	 * [3]   : Arbitration status flag
	 * [2]   : Address-as-slave status flag
	 * [1]   : Address zero status flag
	 * [0]   : Last-received bit status flag
	 */
	//IICSTAT = (2 << 6);

	/* 写从设备地址到IICDS */
	IICDS = (msgs->addr << 1) | (1 << 0);

	/* IICSTAT = 0xB0, start信号，接收数据 */
	IICSTAT = 0xb0;

	/* 调用中断处理函数 */
	while (!msgs->err && (msgs->cnt_tranfer != msgs->len));


	if (p_g_msg->err)
		return -1;
	return 0;
 }

void s3c2440_i2c_tranfer(p_i2c_msg msgs, int num)
{
	int ret_val;
	int i;

	for (i = 0; i < num; i++)
	{
		if (!msgs[i].flags)		/* 写操作 */
			ret_val = do_i2c_tranfer(&msgs[i]);
		else					/* 读操作 */
			ret_val = do_i2c_receive(&msgs[i]);

		if (ret_val)
			return;
	}
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
	register_i2c_controller(&s3c2440_i2c_controller);
}

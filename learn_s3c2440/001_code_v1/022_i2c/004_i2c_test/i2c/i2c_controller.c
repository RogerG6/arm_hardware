# include "i2c_controller.h"
#include "../string_utils.h"
#define I2C_CONTROLLER_NUM 	10	

/* 构造一个i2c_controller数组，存放不同芯片的操作结构体 */
static p_i2c_controller i2c_controllers[I2C_CONTROLLER_NUM];
static p_i2c_controller p_i2c_con_selected;

int register_i2c_controller(p_i2c_controller p)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (!i2c_controllers[i])
		{
			i2c_controllers[i] = p;
			return 0;
		}
	}
	return -1;
}

/* 根据名字，选择是哪个芯片 */
int select_i2c_controller(char * name)
{
	int i;
	for (i = 0; i < I2C_CONTROLLER_NUM; i++)
	{
		if (i2c_controllers[i] && !strcmp(name, i2c_controllers[i]->name))
		{
			p_i2c_con_selected = i2c_controllers[i];
			return 0;
		}
	}
	return -1;
}

/* i2c_tranfer 接口函数 */
void i2c_tranfer(p_i2c_msg msgs, int num)
{
	p_i2c_con_selected->master_tranfer(msgs, num);
}

void i2c_init(void)
{
	//int ret;
	
	/* 注册i2c_controller */
	s3c2440_i2c_add();
	//printf("s3c2440_i2c_add ret = %d\n\r", ret);

	/* 选择其中一款控制器 */
	select_i2c_controller("s3c2440");
	//printf("select_i2c_controller ret = %d\n\r", ret);
	
	/* 调用它的init函数 */
	p_i2c_con_selected->init();
}


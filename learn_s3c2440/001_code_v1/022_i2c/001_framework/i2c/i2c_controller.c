

/* 构造一个i2c_controller数组，存放不同芯片的操作结构体 */
void register_i2c_controller()
{}

/* 根据名字，选择是哪个芯片 */
void select_i2c_controller(char * name)
{}

/* i2c_tranfer 接口函数 */
int i2c_tranfer(i2c_msg msgs, int num)
{}


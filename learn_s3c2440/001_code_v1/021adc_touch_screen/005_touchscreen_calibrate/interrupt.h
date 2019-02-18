

# include "s3c2440_soc.h"
# include "uart.h"

/* 定义一个函数指针数组，用于存放中断例程的函数指针 */
typedef void (* irq_func)(unsigned int);
irq_func irq_arr[60];

void key_irq_func(unsigned int irq);
void register_irq(irq_func pf, unsigned int irq);






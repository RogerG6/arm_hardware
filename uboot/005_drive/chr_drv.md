# 1. 文件说明

first_drv  : led点灯驱动
second_drv : 查询方式按键驱动
third_drv  : 中断方式按键驱动
forth_drv  : poll方式按键驱动
fifth_drv  : fsync按键驱动
sixth_drv  : 同步互斥阻塞按键驱动


===========================================================

# 2. 总结

## 1. led驱动
		1. make生成 first_drv.ko文件
		2. arm-linux-gcc -o first_drv_test first_drv_test.c		/* 生成测试程序 */
		2. cp first_drv.ko first_drv_test /work/first_drv
		3. insmod first_drv.ko				/* 在串口中操作，加载模块 */ 
		4. mknod /dev/xxx c 111 0			/* 手工创建设备节点 */
		   或 在驱动源码中自动创建设备节点(参考busybox中的mdev)
		5. 运行测试程序
		
		===========
		涉及的cmd: rmmod first_drv			/* 卸载驱动模块 */
		
		可见：1. app调用系统函数open时
			  2. open打开/dev/xxx, 根据主设备号111寻找相应的设备，找到后执行驱动程序
			  -----------
			  3. 驱动开发与单片机的开发不同：
					1. 先搭建驱动框架
					2. 单片机直接操作PA，驱动操作VA，需要经过地址映射
					3. 其它就是看原理图、芯片手册
## 2. buttons驱动
		1. 搭建框架
		2. 硬件代码编写
		3. 测试
		------------
		linux命令行：
			top  					/* 相当于win的任务管理器 */
			./second_drv_test &     /* 将second_drv_test切到后台运行 */
			kill PID				/* 结束PID号进程 */

## 3. linux异常和中断框架分析（中断也是一种异常，以它为例进行分析）
	### 1. 发生中断后的操作
			b	vector_irq + stubs_offset	/* 跳到异常向量表 */
											/* 1. 所有异常向量表都由trap.c中trap_init：
											 *    memcpy((void *)vectors, __vectors_start, __vectors_end - __vectors_start);
											 *    memcpy((void *)vectors + 0x200, __stubs_start, __stubs_end - __stubs_start);
											 *    拷贝到0xffff0000的地址
											 * 2. vector_irq用	.macro	vector_stub, name, mode, correction=0宏定义 
											 *    具体内容：保存regs, 切换cpsr，跳到指定地点
											 */
								｜
								｜就是
							   \｜/	
						标号：__irq_svc
           汇编					  irq_handler	/* 宏 */
           ---------------------------------------------------------------------			
           C语言						｜
										｜调用
									    ｜
									   \｜/	
									asm_do_IRQ	/* 所有中断处理的总入口 */
										desc_handle_irq(irq, desc)			/* 根据irq在irq_desc[NUM]数组中找到相应的中断 */
											desc->handle_irq(irq, desc);	/* 执行初始时注册的中断处理函数 */
														｜
														｜就是
													   \｜/	
											/* 以此为例 */
											handle_edge_irq(unsigned int irq, struct irq_desc *desc)	
												handle_IRQ_event(irq, action)		/* 真正的中断处理函数 */
													action->handler(irq, action->dev_id)	/* 逐个执行action中的handler */

	### 2. 初始化中断时的操作
			s3c24xx_init_irq
				set_irq_chip(unsigned int irq, struct irq_chip *chip);	/* en/disable int, 清中断，屏蔽中断…… */
				set_irq_handler(irqno, handle_edge_irq);
					__set_irq_handler(irq, handle, 0, NULL)
					{
						desc->handle_irq = handle;	/* 将传入的handle，name……参数填入irq_desc中 */
						desc->name = name;
					}
				set_irq_flags(irqno, IRQF_VALID);	/* 设置中断标记为VALID，表明可以使用它们 */
				
	### 3. 注册中断
			request_irq(unsigned int irq, irq_handler_t handler, unsigned long irqflags, const char *devname, void *dev_id)
	### 4. 卸载中断
			free_irq(unsigned int irq, void *dev_id)

			
## 4. 按键中断方式
		1. 搭建框架
		2. 硬件代码编写
		3. 测试
		--------------
		insmod third_drv.ko
		exec 5</dev/buttons		/* 启动中断，可以通过cat /proc/interrupts查看
								 * 也可以通过ls -l /proc/770/fd查看（其中770是-sh进程号），可以看到
								 * lr-x------    1 0        0              64 Jan  1 00:05 5 -> /dev/buttons
								 */
		exec 5<&-				/* 关闭中断 */

## 5. poll方式按键驱动
		poll机制：如果没有中断或其他信号，每隔一定时间会返回0，在这段时间内，进程会休眠，进而降低CPU占用率

## 6. 异步通知机制（fsync）
	  查询、中断、poll这三者实现的按键驱动都是由app主动去调用驱动获取键值
	  而异步通知则是当有信号发生时，由驱动主动向app传递键值

## 7. 同步互斥阻塞
	  1. 同步
	  同一时间，有且只能有一个应用程序调用此驱动
	  通过原子操作（原子操作指的是在执行过程中不会被别的代码路径所中断的操作），可以实现
	  -------------
	  运行结果：当进程1在后台运行时，启动的进程2调用和1一样的驱动，结果是打不开/dev/buttons
	
	  2. 互斥
	  信号量（semaphore）是用于保护临界区的一种常用方法，只有申请到信号量的进程才能执行临界区代码。
	  当申请不到信号量时，进程进入休眠等待状态。
	  -------------
	  运行结果：当进程1在后台运行时，启动的进程2调用和1一样的驱动，结果1的状态是S，2是D，当kill 1时，2变成S
	  
	  3. 阻塞与非阻塞
	  阻塞操作：    
	      是指在执行设备操作时若不能获得资源则挂起进程，直到满足可操作的条件后再进行操作。
		  被挂起的进程进入休眠状态，被从调度器的运行队列移走，直到等待的条件被满足。
	  非阻塞操作：  
	      进程在不能进行设备操作时并不挂起，它或者放弃，或者不停地查询，直至可以进行操作为止。





		
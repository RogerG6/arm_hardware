# 1. 文件说明

first_drv  : led点灯驱动
second_drv : 查询方式按键驱动
third_drv  : 中断方式按键驱动
forth_drv  : poll方式按键驱动

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
			
## 3. 按键中断方式
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

## 4. poll方式按键驱动
		poll机制：如果没有中断或其他信号，每隔一定时间会返回0，在这段时间内，进程会休眠，进而降低CPU占用率









		
1、定义全局变量，验证NOR启动只读不可写
2、将data段定位至0x30000000处
3、引入链接脚本，设置data段的relocate & load addr
4、将data段cp至sdram中，定义多个全局变量并初始化，设置相应的lds
5、打印设为0的全局变量，察看其是否为0
6、在lds中定义bss段的start & end, 清除bss段，察看效果
7、提高定位效率，以4BYTEcp，让bss段向4取整
8、重定位整个程序至SDRAM中，重定位前使用位置无关码（相对地址），重定位后可以使用绝对地址
9、用C实现重定位，在汇编中获取参数
10、用C实现重定位，在C中获取参数
11、作业：
(1).使用printf函数打印以下信息:
	volatile unsigned int *dest = (volatile unsigned int *)&__code_start;
	volatile unsigned int *end = (volatile unsigned int *)&__bss_start;
	volatile unsigned int *src = (volatile unsigned int *)0;
a. 打印dest, end, src的值
b. 打印dest, end, src所指向的地址的值

(2). 修改链接脚本，把程序重定位到0x32000000
     再观察上述打印结果
	 


总结：
1、用C实现重定位，从*.lds链接脚本中获取参数，必须用&符号
   原因：在C语言中: int a = 100;
		 其中变量名保存在symbol table中，symbol table中也包含变量100的地址，并且分配内存以存放100，要取出a的地址用&
		 之前 & 取地址原理就是如此
		 
		 类似的，extern int bss_start; 
		 链接脚本中的bss_start的也保存在symbol table中，只不过这次定义在链接脚本中的值，而不是变量的地址，且未分配内存
		 我们要取得其中的值必须用&
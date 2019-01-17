使用链接脚本*.lds

	设置全局变量相应的runtime addr & load addr
		其中runtime addr为重定位后的地址，load addr为重定位前的地址
	当程序需要用到全局变量时，它会从runtime addr处读写数据
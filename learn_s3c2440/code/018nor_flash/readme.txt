001. 识别、打印id，打印各扇区首地址
002. add read from nor flash function
003. add erase, write nor flash function

004. 进入test_nor, disable irq, 测试完毕， enable irq
     使用在C中内嵌汇编码的方式
005，实现和004一样的功能，通过另一种方式：在C中调用汇编码
006. 将scan/read/write/erase进行封装
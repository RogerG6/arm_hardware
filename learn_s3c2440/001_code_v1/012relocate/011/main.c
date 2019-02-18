


# include "init.h"
# include "uart.h"
# include "my_printf.h"
char g_A = 'A'; 

int a = 0;
int b;
short int c = 0;
long d = 0;

extern int text_start, bss_start;   //这些变量在sdram.lds中定义
	volatile unsigned int * load_addr = (volatile unsigned int *)0;
	volatile unsigned int * dest_start = (volatile unsigned int *)&text_start;  //此处必须用&, 语法规定
	volatile unsigned int * dest_end  = (volatile unsigned int *)&bss_start;
	
int main(void)
{
	uart_init();
	
	
	
	//打印dest打印dest, end, src的值
	printf("load_addr = 0x%x, dest_start = 0x%x, dest_end = 0x%x\n\r", load_addr, dest_start, dest_end);
	
    //打印dest, end, src所指向的地址的值
	printf("*load_addr = 0x%x, *dest_start = 0x%x, *dest_end = 0x%x\n\r", *load_addr, *dest_start, *dest_end);
	
	printf("%x\n\r", 0x73a00313);
	
	printf("%d\n\r", -23);

	
	
	return 0;
}







/*
运行结果：


*/






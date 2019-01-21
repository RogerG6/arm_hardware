


# include "init.h"
# include "uart.h"
# include "my_printf.h"

void arm_test(void)
{
	__asm__ __volatile__
	(
#if 0
		"ldr r0, =0x20000050\n"
		//"bl hex_print\n"
		"orr r0, r0, #0xf\n"
		"bl hex_print\n"
		"bl print_space\n"

		"ldr r0, =0x20000050\n"
		//"bl hex_print\n"
		"orr r0, r0, #0xf\n"
		"bl hex_print\n"
		"bl print_space\n"

		"ldr r0, =0x200000d4\n"
		//"bl hex_print\n"
		"orr r0, r0, #0xf\n"
		"bl hex_print\n"
		"bl print_space\n"

		"ldr r0, =0x200000d6\n"
		//"bl hex_print\n"
		"orr r0, r0, #0xf\n"
		"bl hex_print\n"
		"bl print_space\n"
#endif
		"mrs r0, cpsr\n"
		//"bl hex_print\n"              //在orr指令的前一条指令不要用bl调用子程序，否则orr指令结果会将其它位清零
		
		"orr r0, r0, #0xf\n"     
		"bl hex_print\n"

	);
}
#if 1
void irq_disable(void)
{
	__asm__ __volatile__
	(		
		"mrs r0, cpsr\n"
		"bic r0, r0, #0xf\n"    
		"msr cpsr, r0\n"		//get in usr mode
	//	"ldr sp, =0x34000000\n"	 
		
		"mrs r0, cpsr\n"
		"orr r0, r0, #0x80\n"    //disable irq

	//	"bic r0, r0, #0xf\n"      
		"msr cpsr, r0\n"		
		
		"ldr sp, =0x33c00000\n"	  //sp_usr

		"mrs r0, cpsr\n"
		"bl hex_print\n"

	);
}
#endif

int main(void)
{
	//arm_test();
	puts("start\n\r");
	irq_disable();
	puts("end\n\r");
	
	while (1);
	return 0;
}







/*
运行结果：


*/






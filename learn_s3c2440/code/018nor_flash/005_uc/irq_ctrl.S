
AREA Irq_disable, CODE, READONLY, ALIGN=4
EXPORT irq_disable
irq_disable
		mrs r1, cpsr
		bl hex_print
		
		orr r0, r1, #0x80
		bl hex_print
		
		msr cpsr, r0
		
		mrs r0, cpsr
		bl hex_print
		mov pc, lr
		
		end

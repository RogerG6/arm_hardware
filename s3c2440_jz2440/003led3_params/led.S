
.text
.global _start

_start:	
	mov sp,#4096
	mov r0, #4
	bl main
	
	ldr r0,=100000
	bl delay
	
	mov r0, #5
	bl main
	
	ldr r0,=100000
	bl delay
	
	mov r0, #6
	bl main
	
halt:
	b halt
	
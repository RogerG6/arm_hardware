/* Function: s3c2440_soc寄存器头文件 
 * Author: rogerg6
 * 未完待续 */

#ifndef __S3C2440_SOC_H
#define __S3C2440_SOC_H

#define 			__REG(x) 						(*(volatile unsigned int *)(x))
#define 			__REG_BYTE(x) 						(*(volatile unsigned char *)(x))

//IO port
#define 			GPACON 						__REG(0x56000000)   //port A control
#define 			GPBCON 						__REG(0x56000010)   //port B control
#define 			GPCCON 						__REG(0x56000020)   //port C control
#define 			GPDCON 						__REG(0x56000030)   //port D control
#define 			GPECON 						__REG(0x56000040)   //port E control
#define 			GPFCON 						__REG(0x56000050)   //port F control
#define 			GPGCON 						__REG(0x56000060)   //port G control
#define 			GPHCON 						__REG(0x56000070)   //port H control
#define 			GPJCON 						__REG(0x560000D0)   //port J control
#define 			GPADAT 						__REG(0x56000004)   //Port A data
#define 			GPBDAT 						__REG(0x56000014)   //Port B data
#define 			GPCDAT 						__REG(0x56000024)   //Port C data
#define 			GPDDAT 						__REG(0x56000034)   //Port D data
#define 			GPEDAT 						__REG(0x56000044)   //Port E data
#define 			GPFDAT 						__REG(0x56000054)   //Port F data
#define 			GPGDAT 						__REG(0x56000064)   //Port G data
#define 			GPHDAT 						__REG(0x56000074)   //Port H data
#define 			GPJDAT 						__REG(0x560000D4)   //Port J data
#define 			GPBUP 						__REG(0x56000018)   //Pull-up control port B
#define 			GPCUP 						__REG(0x56000028)   //Pull-up control port C
#define 			GPDUP 						__REG(0x56000038)   //Pull-up control port D
#define 			GPEUP 						__REG(0x56000048)   //Pull-up control port E
#define 			GPFUP 						__REG(0x56000058)   //Pull-up control port F
#define 			GPGUP 						__REG(0x56000068)   //Pull-up control port G
#define 			GPHUP 						__REG(0x56000078)   //Pull-up control port H
#define 			GPJUP 						__REG(0x560000D8)   //Pull-up control port J
#define 			MISCCR 						__REG(0x56000080)   //Miscellaneous control
#define 			DCLKCON 					__REG(0x56000084)   //DCLK0/1 control
#define 			EXTINT0 					__REG(0x56000088)   //External interrupt control register 0
#define 			EXTINT1 					__REG(0x5600008C)   //External interrupt control register 1
#define 			EXTINT2 					__REG(0x56000090)   //External interrupt control register 2
#define 			EINTFLT0 					__REG(0x56000094)   //Reserved
#define 			EINTFLT1 					__REG(0x56000098)   //Reserved
#define 			EINTFLT2 					__REG(0x5600009C)   //External interrupt filter control register 2
#define 			EINTFLT3 					__REG(0x560000A0)   //External interrupt filter control register 3
#define 			EINTMASK 					__REG(0x560000A4)   //External interrupt mask
#define 			EINTPEND 					__REG(0x560000A8)   //External interrupt pending
#define 			GSTATUS0 					__REG(0x560000AC)   //External pin status
#define 			GSTATUS1 					__REG(0x560000AC)   //Chip ID
#define 			GSTATUS2 					__REG(0x560000AC)   //Reset status
#define 			GSTATUS3 					__REG(0x560000AC)   //Inform register
#define 			GSTATUS4 					__REG(0x560000AC)   //Inform register
#define 			MSLCON 						__REG(0x560000CC)   //Memory sleep control register

//UART
#define 			ULCON0 					__REG(0x50000000)   //UART channel 0 line control register 
#define 			ULCON1 					__REG(0x50004000)   //UART channel 1 line control register 
#define 			ULCON2 					__REG(0x50008000)   //UART channel 2 line control register 
#define 			UCON0 					__REG(0x50000004)   //UART channel 0 control register
#define 			UCON1 					__REG(0x50004004)   //UART channel 1 control register
#define 			UCON2 					__REG(0x50008004)   //UART channel 2 control register
#define 			UFCON0 					__REG(0x50000008)   //UART channel 0 FIFO control register
#define 			UFCON1 					__REG(0x50004008)   //UART channel 1 FIFO control register
#define 			UFCON2 					__REG(0x50008008)   //UART channel 2 FIFO control register
#define 			UMCON0 					__REG(0x5000000C)   //UART channel 0 Modem control register
#define 			UMCON1 					__REG(0x5000400C)   //UART channel 1 Modem control register
#define 			UTRSTAT0 				__REG(0x50000010)   //UART channel 0 Tx/Rx status register
#define 			UTRSTAT1 				__REG(0x50004010)   //UART channel 1 Tx/Rx status register
#define 			UTRSTAT2 				__REG(0x50008010)   //UART channel 2 Tx/Rx status register
#define 			UERSTAT0 				__REG(0x50000014)   //UART channel 0 Rx error status register
#define 			UERSTAT1 				__REG(0x50004014)   //UART channel 1 Rx error status register
#define 			UERSTAT2 				__REG(0x50008014)   //UART channel 2 Rx error status register
#define 			UFSTAT0 				__REG(0x50000018)   //UART channel 0 FIFO status register
#define 			UFSTAT1 				__REG(0x50004018)   //UART channel 1 FIFO status register
#define 			UFSTAT2 				__REG(0x50008018)   //UART channel 2 FIFO status register
#define 			UMSTAT0 				__REG(0x5000001C)   //UART channel 0 modem status register
#define 			UMSTAT1				    __REG(0x5000401C)   //UART channel 1 modem status register
#define 			UTXH0				    __REG_BYTE(0x50000020)   //UART channel 0 transmit buffer register(L)
#define 			UTXH1				    __REG_BYTE(0x50004020)   //UART channel 1 transmit buffer register(L)
#define 			UTXH2				    __REG_BYTE(0x50008020)   //UART channel 2 transmit buffer register(L)
#define 			URXH0				    __REG_BYTE(0x50000024)   //UART channel 0 receive buffer register(L)
#define 			URXH1				    __REG_BYTE(0x50004024)   //UART channel 1 receive buffer register(L)
#define 			URXH2				    __REG_BYTE(0x50008024)   //UART channel 2 receive buffer register(L)
//#define 			UTXH2				    __REG_BYTE(0x50008023)   //UART channel 2 transmit buffer register(B)
//#define 			UTXH1				    __REG_BYTE(0x50004023)   //UART channel 1 transmit buffer register(B)
//#define 			UTXH0				    __REG_BYTE(0x50000023)   //UART channel 0 transmit buffer register(B)
//#define 			URXH1				    __REG_BYTE(0x50004027)   //UART channel 1 receive buffer register(B)
//#define 			URXH0				    __REG_BYTE(0x50000027)   //UART channel 0 receive buffer register(B)
//#define 			URXH2				    __REG_BYTE(0x50008027)   //UART channel 2 receive buffer register(B)
#define 			UBRDIV0				    __REG(0x50000028)   //Baud rate divisior register 0
#define 			UBRDIV1				    __REG(0x50004028)   //Baud rate divisior register 1
#define 			UBRDIV2				    __REG(0x50008028)   //Baud rate divisior register 2

//memory contorl
#define 			BWSCON				    __REG(0x48000000)   //Bus width & wait status control register
#define 			BANKCON0				__REG(0x48000004)   //Bank 0 control register
#define 			BANKCON1				__REG(0x48000008)   //Bank 1 control register
#define 			BANKCON2				__REG(0x4800000C)   //Bank 2 control register
#define 			BANKCON3				__REG(0x48000010)   //Bank 3 control register
#define 			BANKCON4				__REG(0x48000014)   //Bank 4 control register
#define 			BANKCON5				__REG(0x48000018)   //Bank 5 control register
#define 			BANKCON6				__REG(0x4800001C)   //Bank 6 control register   sdram
#define 			BANKCON7				__REG(0x48000020)   //Bank 7 control register	sdram
#define 			REFRESH					__REG(0x48000024)   //SDRAM refresh control register
#define 			BANKSIZE				__REG(0x48000028)   //Flexible bank size register 
#define 			MRSRB6					__REG(0x4800002C)   //Mode register set register bank6
#define 			MRSRB7					__REG(0x48000030)   //Mode register set register bank7

//USB Host Controller
#define 			HcRevision						__REG(0x49000000)   //Control and status group
#define 			HcControl						__REG(0x49000004)
#define 			HcCommonStatus					__REG(0x49000008)
#define 			HcInterruptStatus				__REG(0x4900000C)
#define 			HcInterruptEnable				__REG(0x49000010)
#define 			HcInterruptDisable				__REG(0x49000014)
#define 			HcHCCA							__REG(0x49000018)   //Memory pointer group
#define 			HcPeriodCuttentED				__REG(0x4900001C)
#define 			HcControlHeadED					__REG(0x49000020)
#define 			HcControlCurrentED				__REG(0x49000024)
#define 			HcBulkHeadED					__REG(0x49000028)
#define 			HcBulkCurrentED					__REG(0x4900002C)
#define 			HcDoneHead						__REG(0x49000030)   //Frame counter group
#define 			HcRmInterval					__REG(0x49000034)
#define 			HcFmRemaining					__REG(0x49000038)
#define 			HcFmNumber						__REG(0x4900003C)
#define 			HcPeriodicStart					__REG(0x49000040)
#define 			HcLSThreshold					__REG(0x49000044)
#define 			HcRhDescriptorA					__REG(0x49000048)   //Root hub group
#define 			HcRhDescriptorB					__REG(0x4900004C) 
#define 			HcRhStatus						__REG(0x49000050) 
#define 			HcRhPortStatus1					__REG(0x49000054) 
#define 			HcRhPortStatus2					__REG(0x49000058) 

//Interrupt Controller
#define 			SRCPND						__REG(0X4A000000) 			//Interrupt request status
#define 			INTMOD						__REG(0X4A000004) 			//Interrupt mode control
#define 			INTMSK						__REG(0X4A000008) 			//Interrupt mask control
#define 			PRIORITY					__REG(0X4A00000C) 			//IRQ priority control
#define 			INTPND						__REG(0X4A000010) 			//Interrupt request status
#define 			INTOFFSET					__REG(0X4A000014) 			//Interrupt request source offset
#define 			SUBSRCPND					__REG(0X4A000018) 			//Sub source pending
#define 			INTSUBMSK					__REG(0X4A00001C) 			//Interrupt sub mask

//Clock & Power Management
#define 			LOCKTIME				__REG(0x4C000000) 			//PLL lock time counter
#define 			MPLLCON					__REG(0x4C000004) 			//MPLL control
#define 			UPLLCON					__REG(0x4C000008) 			//UPLL control
#define 			CLKCON					__REG(0x4C00000C) 			//Clock generator control
#define 			CLKSLOW					__REG(0x4C000010) 			//Slow clock control
#define 			CLKDIVN					__REG(0x4C000014) 			//Clock divider control
#define 			CAMDIVN					__REG(0x4C000018) 			//Camera clock divider control


//PWM Timer
#define 			TCFG0					__REG(0x51000000) 			//Timer configuration
#define 			TCFG1					__REG(0x51000004) 			//Timer configuration
#define 			TCON					__REG(0x51000008) 			//Timer control
#define 			TCNTB0					__REG(0x5100000C) 			//Timer count buffer 0
#define 			TCMPB0					__REG(0x51000010) 			//Timer compare buffer 0
#define 			TCNTO0					__REG(0x51000014) 			//Timer count observation 0
#define 			TCNTB1					__REG(0x51000018) 			//Timer count buffer 1
#define 			TCMPB1					__REG(0x5100001C) 			//Timer compare buffer 1
#define 			TCNTO1					__REG(0x51000020) 			//Timer count observation 1
#define 			TCNTB2					__REG(0x51000024) 			//Timer count buffer 2
#define 			TCMPB2					__REG(0x51000028) 			//Timer compare buffer 2
#define 			TCNTO2					__REG(0x5100002C) 			//Timer count observation 2
#define 			TCNTB3					__REG(0x51000030) 			//Timer count buffer 3
#define 			TCMPB3					__REG(0x51000034) 			//Timer compare buffer 3
#define 			TCNTO3					__REG(0x51000038) 			//Timer count observation 3
#define 			TCNTB4					__REG(0x5100003C) 			//Timer count buffer 4
#define 			TCNTO4					__REG(0x51000040) 			//Timer count observation 4


#endif






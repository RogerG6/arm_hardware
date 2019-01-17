/* Function: s3c2440_soc寄存器头文件 
 * Author: rogerg6
 * 未完待续 */

#ifndef __S3C2440_SOC_H
#define __S3C2440_SOC_H

#define 			__REG(x) 						(*(volatile unsigned int *)(x))
#define 			__REG_B(x) 						(*(volatile unsigned char *)(x))
//GPIO
#define 			GPHCON 					__REG(0x56000070)   //port H control
#define 			GPHUP 					__REG(0x56000078)   //Pull-up control port H

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
#define 			UTXH0				    __REG_B(0x50000020)   //UART channel 0 transmit buffer register(L)
#define 			UTXH1				    __REG_B(0x50004020)   //UART channel 1 transmit buffer register(L)
#define 			UTXH2				    __REG_B(0x50008020)   //UART channel 2 transmit buffer register(L)
#define 			URXH0				    __REG_B(0x50000024)   //UART channel 0 receive buffer register(L)
#define 			URXH1				    __REG_B(0x50004024)   //UART channel 1 receive buffer register(L)
#define 			URXH2				    __REG_B(0x50008024)   //UART channel 2 receive buffer register(L)
//#define 			UTXH2				    __REG_B(0x50008023)   //UART channel 2 transmit buffer register(B)
//#define 			UTXH1				    __REG_B(0x50004023)   //UART channel 1 transmit buffer register(B)
//#define 			UTXH0				    __REG_B(0x50000023)   //UART channel 0 transmit buffer register(B)
//#define 			URXH1				    __REG_B(0x50004027)   //UART channel 1 receive buffer register(B)
//#define 			URXH0				    __REG_B(0x50000027)   //UART channel 0 receive buffer register(B)
//#define 			URXH2				    __REG_B(0x50008027)   //UART channel 2 receive buffer register(B)
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

#endif






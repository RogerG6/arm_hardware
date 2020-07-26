/* 参考：
 * drivers/mtd/nand/s3c2410.c
 * drivers/mtd/nand/at91_nand.c
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
 
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>
 
#include <asm/io.h>
 
#include <asm/arch/regs-nand.h>
#include <asm/arch/nand.h>

static struct nand_chip	* s3c_nand;
static struct mtd_info * mtd;

static struct mtd_partition s3c_nand_parts[] = {
	[0] = {
        .name   = "bootloader",
        .size   = 0x00040000,
		.offset	= 0,
	},
	[1] = {
        .name   = "params",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00020000,
	},
	[2] = {
        .name   = "kernel",
        .offset = MTDPART_OFS_APPEND,
        .size   = 0x00200000,
	},
	[3] = {
        .name   = "root",
        .offset = MTDPART_OFS_APPEND,
        .size   = MTDPART_SIZ_FULL,
	}
};

struct s3c_regs {
	unsigned long nfconf  ;
	unsigned long nfcont  ;
	unsigned long nfcmd   ;
	unsigned long nfaddr  ;
	unsigned long nfdata  ;
	unsigned long nfecc0 ;
	unsigned long nfecc1 ;
	unsigned long nfecc  ;
	unsigned long nfstat  ;
	unsigned long nfestat0;
	unsigned long nfestat1;
	unsigned long nfmecc0 ;
	unsigned long nfmecc1 ;
	unsigned long nfsecc  ;
	unsigned long nfsblk  ;
	unsigned long nfeblk  ;	
};
static struct s3c_regs *s3c_regs;

static void s3c_nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	if (ctrl & NAND_CLE)
	{
		/* 发命令: NFCMMD=dat */
		s3c_regs->nfcmd = dat;
	}
	else
	{
		/* 发地址: NFADDR=dat */
		s3c_regs->nfaddr = dat;
	}
}

static void s3c_nand_select(struct mtd_info *mtd, int chipnr)
{
	if (chipnr == -1)
	{
		/* 取消选中 */
		s3c_regs->nfcont |= (1<<1);
	}
	else
	{
		/* 选中芯片 */		
		s3c_regs->nfcont &= ~(1<<1);	}

}

static int s3c_dev_ready(struct mtd_info *mtd)
{
	return (s3c_regs->nfstat & (1<<0));
}



static int s3c_nand_init(void)
{
	struct clk * clk;
	
	/* 1. 分配nand_chip */
	s3c_nand = kzalloc(sizeof(struct nand_chip), GFP_KERNEL);

	s3c_regs = ioremap(0x4E000000, sizeof(struct s3c_regs));
	
	/* 2. 设置nand_chip是用来确定一些函数
	 *    选中，发送，读写，判断状态
	 */
	s3c_nand->select_chip = s3c_nand_select; 
	s3c_nand->cmd_ctrl    = s3c_nand_cmd_ctrl;
	s3c_nand->IO_ADDR_R   = &s3c_regs->nfdata;
	s3c_nand->IO_ADDR_W   = &s3c_regs->nfdata;
	s3c_nand->dev_ready   = s3c_dev_ready;
	 
	/* 3. 硬件相关设置 */
	/* 使能nand 时钟 */
	clk = clk_get(NULL, "nand");
	clk_enable(clk);              /* CLKCON'bit[4] */

	/* 设置nand时序 */
	/* HCLK=100MHz
	 * TACLS:  发出CLE/ALE之后多长时间才发出nWE信号, 从NAND手册可知CLE/ALE与nWE可以同时发出,所以TACLS=0
	 * TWRPH0: nWE的脉冲宽度, HCLK x ( TWRPH0 + 1 ), 从NAND手册可知它要>=12ns, 所以TWRPH0>=1
	 * TWRPH1: nWE变为高电平后多长时间CLE/ALE才能变为低电平, 从NAND手册可知它要>=5ns, 所以TWRPH1>=0
	 */
#define TACLS 0
#define TWRPH0 1
#define TWRPH1 0
	s3c_regs->nfconf = ((TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4));

	/* NFCONT: 
	 * BIT1-设为1, 取消片选 
	 * BIT0-设为1, 使能NAND FLASH控制器
	 */
	s3c_regs->nfcont = (1<<1) | (1<<0);

	/* enable ECC */
	s3c_nand->ecc.mode = NAND_ECC_SOFT;	

	/* 4.1 使用：nand_scan */
	mtd = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);
	mtd->owner = THIS_MODULE;
	mtd->priv  = s3c_nand;
	nand_scan(mtd, 1);
	
	/* 4.2 add_mtd_partitons */
	add_mtd_partitions(mtd, s3c_nand_parts, 4);
	
	return 0;
}

static void s3c_nand_exit(void)
{
	del_mtd_partitions(mtd);
	kfree(mtd);
	iounmap(s3c_regs);
	kfree(s3c_nand);
}

module_init(s3c_nand_init);
module_exit(s3c_nand_exit);
MODULE_LICENSE("GPL");


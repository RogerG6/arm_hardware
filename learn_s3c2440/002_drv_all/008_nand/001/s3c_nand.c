/* 参考：
 * drivers/mtd/nand/s3c2410.c
 * drivers/mtd/nand/at91_nand.c
 */

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>
#include <asm/sizes.h>

#include <asm/hardware.h>
#include <asm/arch/board.h>
#include <asm/arch/gpio.h>

static struct nand_chip	* s3c_nand;
static struct mtd_info * mtd;

static void s3c_nand_cmd_ctrl(struct mtd_info *mtd, unsigned int command,
			 int column, int page_addr)
{
}

static void s3c_nand_select(struct mtd_info *mtd, int chipnr)
{
	if (chipnr == -1)
	{
		/* 取消选中 */
	}
	else
	{
		/* 选中芯片 */		
	}

}

static int s3c_dev_ready(struct mtd_info *mtd)
{
	return "NFSTAT的bit[0]";
}



static int s3c_nand_init(void)
{
	/* 1. 分配nand_chip */
	s3c_nand = kzalloc(sizeof(struct nand_chip), GFP_KERNEL);
	
	/* 2. 设置nand_chip是用来确定一些函数
	 *    选中，发送，读写，判断状态
	 */
	s3c_nand->select_chip = s3c_nand_select; 
	s3c_nand->cmd_ctrl = s3c_nand_cmd_ctrl;
	s3c_nand->IO_ADDR_R   = "NFDATA的虚拟地址";
	s3c_nand->IO_ADDR_W   = "NFDATA的虚拟地址";
	s3c_nand->dev_ready   = s3c_dev_ready;
	 
	/* 3. 硬件相关设置 */
	/* 4.1 使用：nand_scan */
	mtd = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);
	mtd->owner = THIS_MODULE;
	mtd->priv  = s3c_nand;
	nand_scan(mtd, 1);
	
	/* 4.2 add_mtd_partitons */
	return 0;
}

static void s3c_nand_exit(void)
{
}

module_init(s3c_nand_init);
module_exit(s3c_nand_exit);
MODULE_LICENSE("GPL");


/*
 * 参考/drivers/mtd/maps/physmap.c
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <asm/io.h>

static struct mtd_partition s3c_nor_parts[] = {
	[0] = {
        .name   = "bootloader_nor",
        .size   = 0x00040000,
		.offset	= 0,
	},
	[1] = {
        .name   = "root_nor",
        .offset = MTDPART_OFS_APPEND,
        .size   = MTDPART_SIZ_FULL,
	}
};


static struct map_info * s3c_nor_map;
static struct mtd_info * s3c_nor_mtd;

static int s3c_nor_init(void)
{
	/* 1. 分配一个map_info结构体 */
	s3c_nor_map = kzalloc(sizeof(struct map_info), GFP_KERNEL);
	
	/* 2. 设置
	 *    物理地址phys/大小size/位宽bankwidth/虚拟地址virt
	 */
	s3c_nor_map->name = "s3c_nor";
	s3c_nor_map->phys = 0;
	s3c_nor_map->size = 0x1000000;
	s3c_nor_map->bankwidth = 2;
	//s3c_nor_map->set_vpp = physmap_data->set_vpp;

	s3c_nor_map->virt = ioremap(s3c_nor_map->phys, s3c_nor_map->size);

	simple_map_init(s3c_nor_map);

	/* 3. 使用 do_map_probe */
	printk("cfi_probe\n");
	s3c_nor_mtd = do_map_probe("cfi_probe", s3c_nor_map);
	if (!s3c_nor_mtd)
	{
		printk("jedec_probe\n");
		s3c_nor_mtd = do_map_probe("jedec_probe", s3c_nor_map);
	}

	if (!s3c_nor_mtd)
	{
		iounmap(s3c_nor_map->virt);
		kfree(s3c_nor_map);
		return -1;
	}
	
	/* 4. add_mtd_partitions */
	add_mtd_partitions(s3c_nor_mtd, s3c_nor_parts, 2);
	
	return 0;
}

static void s3c_nor_exit(void)
{
	del_mtd_partitions(s3c_nor_mtd);
	iounmap(s3c_nor_map->virt);
	kfree(s3c_nor_map);
}

module_init(s3c_nor_init);
module_exit(s3c_nor_exit);

MODULE_LICENSE("GPL");



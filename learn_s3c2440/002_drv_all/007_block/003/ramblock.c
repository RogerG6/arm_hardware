/* 参考：
 * drivers/block/xd.c
 * drivers\block\z2ram.c
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>

static struct gendisk *ramdisk;
static int major;
static struct request_queue *ramblock_queue;
static DEFINE_SPINLOCK(ramblock_lock);
static char * ramblock_buf;

static struct block_device_operations ramblock_fops = {
	.owner	= THIS_MODULE,
};


#define BLK_SIZE (1024*1024)

static void do_ramblock_request (request_queue_t * q)
{
	struct request *req;
	static int cnt = 0;
	
	//printk("do_ramblock_request %d\n", ++cnt);
	while ((req = elv_next_request(q)) != NULL) {
		/* 数据传递3要素： */
		/* 1. 源/目的 */		
		unsigned int offset = req->sector / 512;

		/* 2. 目的/源 */

		/* 3. 长度   */
		unsigned int len = req->current_nr_sectors / 512;

		if (rq_data_dir(req) == READ)
		{
			memcpy(req->buffer, ramblock_buf+offset, len);
		}
		else
		{
			memcpy(ramblock_buf+offset, req->buffer, len);		}
		
		end_request(req, 1);
	}

}


static int ramblock_init(void)
{
	/* 1. 分配gendisk */
	ramdisk = alloc_disk(16);

	/* 2. 设置 */
	/* 2.1 分配/设置队列 */
	ramblock_queue = blk_init_queue(do_ramblock_request, &ramblock_lock);	
	ramdisk->queue = ramblock_queue;

	/* 2.3 设置gendisk其他信息 */
	major = register_blkdev(0, "ramblock");
	
	ramdisk->major = major;
	ramdisk->first_minor = 0;
	sprintf(ramdisk->disk_name, "ramblock");
	ramdisk->fops = &ramblock_fops;
	set_capacity(ramdisk, BLK_SIZE / 512);

	/* 3. 硬件相关操作 */
	ramblock_buf = kzalloc(BLK_SIZE, GFP_KERNEL);
	
	/* 4. 注册 */
	add_disk(ramdisk);
	
	return 0;
}

static void ramblock_exit(void)
{
	unregister_blkdev(major, "ramblock");
	del_gendisk(ramdisk);
	put_disk(ramdisk);
	blk_cleanup_queue(ramblock_queue);
	kfree(ramblock_buf);

}

module_init(ramblock_init);
module_exit(ramblock_exit);
MODULE_LICENSE("GPL");


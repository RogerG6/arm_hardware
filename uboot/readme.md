# 1. 文件说明

001. 一个简单的bootloader, 针对JZ2440
002. 由于001启动后，启动内核的时间太长，大部分时间都花费在拷贝uImage到sdram上
	 因此，启动icache功能，加速
003. 内核配置、编译流程
		1. 解压内核压缩包
		   tar xjf linux-2.6.22.6.tar.bz2
		2. 打上补丁
		   cd linux-2.6.22.6
		   patch -p1 < ../linux-2.6.22.6_jz2440.patch	/* p1:补丁打到哪个路径 */
		3. 编译
		   3.1 把4.3寸LCD的源码覆盖原来的3.5寸源码
				mv ../4.3寸LCD_mach-smdk2440.c  arch/arm/mach-s3c2440/mach-smdk2440.c  /* 这个uImage可以用在4.3寸LCD屏上 */
		   3.2 
				使用默认设置
					make s3c2410_defconfig	/* 可以用find -name "*defconfig*"搜索与当前开发板相近的配置文件 */
					make menuconfig			/* 可视化的图形菜单配置 */
					make uImage 			/* 制作uImage(64B header + real kernel) */
				使用厂家设置
					cp config_ok .config	/* config_ok是厂家提供的适用于相应的板子的配置文件，我们只需要将其替换成.config就行了 */
					make menuconfig
					make uImage


＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
# 2. 总结

## 1. 内核配置过程分析
	  以网卡配置项CONFIG_DM9000为例：
	  1. 执行make uImage
	  2. .config文件生成以下2个文件：
				include/config/auto.conf：CONFIG_DM9000=y    	    /* 被顶层Makefile包含 */
				include/linux/autoconf.h：#define CONFIG_DM9000 1	/* 被内核源码使用 */
	  3. drivers/net/Makefile：obj-$(CONFIG_DM9000) += dm9dev9000c.o	/* if $(CONFIG_DM9000) = y, then DM9000将被编译进内核
																		 * if $(CONFIG_DM9000) = m, then DM9000将被编译成模块
																		 * 而$(CONFIG_DM9000)的值在上述auto.conf中定义
																		 */

## 2. 内核启动流程分析
	  从linux-2.6.22.6\arch\arm\kernel\head.S开始分析，以下缩进代表调用，同级缩进数一样，展开为{}：
	  
	head.S  （linux-2.6.22.6\arch\arm\kernel\head.S）
		__lookup_processor_type		/* 识别处理器号 */
		__lookup_machine_type		/* 识别机器号, 处理uboot传入的mach id */
		__create_page_tables		/* 创建页表 */
		__enable_mmu				/* 使能MMU */
		__switch_data	（linux-2.6.22.6\arch\arm\kernel\head-common.S）
		{
			/* Copy data segment if needed */
			/* Clear BSS (and zero fp) */
			/* Save processor ID */
			/* Save machine type */
			/* Clear 'A' bit */
			/* Save control register values */
			start_kernel	（linux-2.6.22.6\init\main.c）
			{
				...
				/* 打印linux版本信息 */
				...
				
				setup_arch(&command_line)			/* 解析uboot传入的cmdline信息 */
					parse_tag()
					parse_commandline()
				setup_command_line(command_line)	/* 解析uboot传入的cmdline信息 */
				parse_early_param
					do_early_param					/* 处理uboot传入的参数，从__setup_start到__setup_end，调用early函数 */
				unknown_bootoption
					obsolete_checksetup				/* 处理uboot传入的参数，从__setup_start到__setup_end，调用非early函数 */
						
				...
				/* 一系列初始化 */
				...
				
				rest_init()
					kernel_init()
						prepare_namespace()
							mount_root()			/* 挂接根文件系统 */
						init_post()					/* 执行应用程序 */
			}               |
							|
		}					|
							|
============================|==================================================================
## 3. 根文件系统启动第一个应|用程序							
						   \|/
						init_post()	
						{
							/* 开启终端的标准输入、输出、错误，在这里标准口是uart0 */
							sys_open((const char __user *) "/dev/console", O_RDWR, 0) < 0)
							(void) sys_dup(0);
							(void) sys_dup(0);
							
							/* 若有命令行输入，则执行命令行的命令
 							 * 否则，执行下一条命令，正常情况下不会返回，而是一直在循环
							 */
							run_init_process(execute_command)	
							run_init_process("/sbin/init");
							run_init_process("/etc/init");
							run_init_process("/bin/init");
							run_init_process("/bin/sh");
						}

=================================================================
## 4. 根文件系统init进程分析

	/********************************************************
		inittab的格式：
		<id>:<runlevels>:<action>:<process>
		id => /dev/id ：用作终端(stdin, stdout, stderr )
		runlevels     : 忽略
		action   	  : 执行时机(合法命令：sysinit, respawn, askfirst, wait, once, restart, ctrlaltdel, shutdown)
		process   	  : 应用程序或脚本（可执行的文件）
	*********************************************************/

	在启动linux后，我们在控制台输入如ls, cd, cp等等命令时，其实都是通过busybox来实现的
	接下来分析busybox：

	busybox-1.7.0\init\init.c
		init_main()
		{
			parse_inittab()
			{
				file = fopen(INITTAB, "r")	/* 打开配置文件etc/inittab */
				...
				new_init_action(action, process, id)
				{
					struct init_action *new_action	/* 创建一个init_action结构体，并填充 */
					init_action_list				/* 并在这个链表中加入该结构体 */
					……
				}
			}
			run_actions(SYSINIT)
				waitfor(a, 0);				/* 执行应用程序，等待它执行完毕 */
					run(a)						/* 创建子进程 */
					waitpid(runpid, &status, 0) /* 等待它结束 */
				delete_init_action(a);			/* 在init_action_list链表中删除 */
			run_actions(WAIT)
				waitfor(a, 0);				/* 执行应用程序，等待它执行完毕 */
					run(a)						/* 创建子进程 */
					waitpid(runpid, &status, 0) /* 等待它结束 */
				delete_init_action(a);			/* 在init_action_list链表中删除 */
			run_actions(ONCE)
				run(a)						/* 创建子进程 */
				delete_init_action(a);		/* 在init_action_list链表中删除 */
			while(1)
			{
				run_actions(RESPAWN)
					if (a->pid == 0)
						a->pid = run(a);
								-------
									打印：Please press Enter to activate this console. 
									等待回车
									创建子进程
				run_actions(ASKFIRST)
					if (a->pid == 0)
						a->pid = run(a);
				wpid = wait(NULL)			/* 等待子进程退出 */
				while (wpid > 0)
				{
					a->pid = 0;				/* 退出后，设置pid = 0 */
				}
			}
		}

	--------------
	总结：	
		一个最小的根文件系统所需的项：
			1. /dev/console  /dev/null		/* 终端，如果没有终端则默认为/dev/null */
			2. /etc/inittab					/* 配置文件 */
			3. 配置文件中指定的应用程序		/* 即其中的process */
			4. C库							/* 如printf, scanf, fopen等等函数 */
			5. init本身，即busybox
		
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
## 5. 构造根文件系统之busybox

	  1. 我们在此做一个最简单的根文件系统：
			1. /dev/console  /dev/null		/* 终端，如果没有终端则默认为/dev/null */
					ls /dev/console /dev/null
					mkdir first_fs			/* 创建根文件系统目录 */
					cd first_fs
					mkdir dev
					mknod cosole c 5 1		/* 创建节点 */
					mknod null c 1 3
			2. /etc/inittab					/* 配置文件 */
					mkdir /first_fs/etc
					在/etc目录下创建inittab，在其中写入console::askfirst:-/bin/sh
			3. 配置文件中指定的应用程序		/* 即其中的process */
			4. C库							/* 如printf, scanf, fopen等等函数 */
					cd /work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/lib
					mkdir /work/first_fs/lib
					cp *.so* /work/first_fs/lib/ -d		/* 将库中的所有.so文件cpy到目标目录 */
			5. init本身，即busybox
					tar xjf busybox-1.7.0.tar.bz2
					make menuconfig		/* 配置busybox */
					make				/* 编译 */
					make CONFIG_PREFIX=<dest dir> install	/* 安装，切不可make install */
      
	  2. 制作fs镜像文件（yaffs2）
			tar xjf yaffs_source_util_larger_small_page_nand.tar.bz2
			cd Development_util_ok/yaffs2/utils/
			make	生成此工具：mkyaffs2image
			sudo cp mkyaffs2image /usr/local/bin/		/* 将此工具cpy到此系统目录 */
			sudo chmod +x /usr/local/bin/mkyaffs2image	/* 给mkyaffs2image加上可执行属性 */
			cd /work									/* 切到first_fs的上一层目录 */
			mkyaffs2image first_fs first_fs.yaffs2		/* 生成镜像文件first_fs.yaffs2 */
			将上述镜像文件烧写到开发板
			
	  3. 创建proc（虚拟文件系统）
			方法1：
			/* 其实，rcS是一个脚本，当系统启动时，会执行它 */
			mkdir proc    /* 在first_fs目录创建proc目录 */
			在inittab文件中加入：  ::sysinit:/etc/init.d/rcS
			同时在/etc/init.d/rcS文件中加入：mount -t proc none /proc
			
			方法2：
			/* 其实，rcS是一个脚本，当系统启动时，会执行其中的mount -a
 			 * mount -a会根据etc/fstab中的内容加载proc
			 */
			mkdir proc    /* 在first_fs目录创建proc目录 */
			在rcS文件中加入：mount -a
			在etc/fstab文件中加入：proc  /proc  proc  defaults  0  0
			
	  4. 自动创建/dev目录下的设备文件
			使用busybox中的mdev工具，它会自动创建/dev/设备节点
			在etc/fstab文件中加入：
				proc    /proc   proc    defaults        0       0
				sysfs   /sys    sysfs   defaults        0       0
				tmpfs   /dev    tmpfs   defaults        0       0
			在/etc/init.d/rcS文件中加入：
				mount -a 
				mkdir /dev/pts
				mount -t devpts devpts /dev/pts
				echo /sbin/mdev > /proc/sys/kernel/hotplug
				mdev -s
			制作fs.yaffs2镜像文件，烧写
	  
	  自此，最小的根文件系统制作完毕 

＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	   5. 制作fs镜像文件（jffs2，压缩格式，文件大小比yaffs2小）
			1. 安装zlib
				tar xzf zlib-1.2.3.tar.gz
				cd zlib-1.2.3
				./configure --shared --prefix=/usr
				make
				sudo make install
			2. 编译mkfs.jffs2
				tar xjf mtd-utils-05.07.23.tar.bz2
				cd mtd-utils-05.07.23/util
				make
				sudo make install
			3. 切到根文件系统的上一层目录执行：
				mkfs.jffs2 -n -s 2048 -e 128KiB -d first_fs -o first_fs.jffs2
			4. 烧写
			   如果挂接失败，则设置bootargs=noinitrd root=/dev/mtdblock3 rootfstype=jffs2 init=/linuxrc console=ttySAC0,115200

＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	   6. 制作NFS文件系统 
			1. 手工挂接NFS：从flash上启动根文件系统，再用命令挂接NFS
				1. 服务器“允许”目标根文件目录可被挂接
					在/etc/exports中加入：/work/first_fs  *(rw,sync,no_root_squash)
					重启nfs: sudo /etc/init.d/nfs-kernel-server restart
				2. 设置单板去挂接
					mkdir /mnt
					mount -t nfs -o nolock 192.168.0.103:/work/first_fs /mnt
				这样，服务器端修改或创建的/work/first_fs(必须是此目录下的)，可以实时的同步到单板的/mnt目录中
				
			2. 直接从NFS启动
				1. 服务器“允许”目标根文件目录可被挂接
					在/etc/exports中加入：/work/first_fs  *(rw,sync,no_root_squash)
					重启nfs: sudo /etc/init.d/nfs-kernel-server restart
				2. 设置bootargs=noinitrd root=/dev/nfs nfsroot=192.168.0.103:/work/first_fs ip=192.168.0.99:192.168.0.103:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC0,115200
				   参数设置在内核源码/documetation/nfsroot.txt中
				3. 在开发板上启动内核，即可。
				   这样，我们在服务器上开发，自动同步，就可以在板子上做试验，方便！！！



===========================================================================
===========================================================================
制作NFS文件系统总结：
1. init本身，即busybox
		tar xjf busybox-1.7.0.tar.bz2
		make menuconfig		/* 配置busybox */
		make				/* 编译 */
		make CONFIG_PREFIX=<dest dir> install	/* 安装，切不可make install */
2. /dev/console  /dev/null		/* 终端，如果没有终端则默认为/dev/null */
		ls /dev/console /dev/null
		mkdir first_fs			/* 创建根文件系统目录 */
		cd first_fs
		mkdir dev
		mknod cosole c 5 1		/* 创建节点 */
		mknod null c 1 3
3. /etc/inittab					/* 配置文件 */
		mkdir /first_fs/etc
		在/etc目录下创建inittab，在其中写入
			console::askfirst:-/bin/sh
			::sysinit:/etc/init.d/rcS	/* 根据/etc/init.d/rcS这个脚本文件来初始化 */
   /etc/init.d/rcS中加入：
		mount -a
		mkdir /dev/pts
		mount -t devpts devpts /dev/pts
		echo /sbin/mdev > /proc/sys/kernel/hotplug	/* 使用busybox中的mdev工具，它会自动创建/dev/设备节点 */
		mdev -s
   /etc/fstab中加入：
		#device         mount-point             type    options         dump    fsck    order
		proc            /proc                   proc    defaults        0               0
		sysfs           /sys                    sysfs   defaults        0               0
		tmpfs           /dev                    tmpfs   defaults        0               0


4. 配置文件中指定的应用程序		/* 即其中的process */
5. C库							/* 如printf, scanf, fopen等等函数 */
		cd /work/tools/gcc-3.4.5-glibc-2.3.6/arm-linux/lib
		mkdir /work/first_fs/lib
		cp *.so* /work/first_fs/lib/ -d		/* 将库中的所有.so文件cpy到目标目录 */
6. 从NFS启动
		1. 服务器“允许”目标根文件目录可被挂接
			在/etc/exports中加入：/work/first_fs  *(rw,sync,no_root_squash)
			重启nfs: sudo /etc/init.d/nfs-kernel-server restart
		2. 设置bootargs=noinitrd root=/dev/nfs nfsroot=192.168.0.103:/work/first_fs ip=192.168.0.99:192.168.0.103:192.168.0.1:255.255.255.0::eth0:off init=/linuxrc console=ttySAC0,115200
		   参数设置在内核源码/documetation/nfsroot.txt中
		3. 在开发板上启动内核，即可。
		   这样，我们在服务器上开发，自动同步，就可以在板子上做试验，方便！！！





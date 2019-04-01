#include "setup.h"

extern void puts(const char * s);
extern void uart0_init(void);
extern void nand_read(unsigned int src, unsigned char * dest, unsigned int len);
extern void puthex(unsigned int val);

static struct tag *params;

void setup_start_tag(void)
{
	params = (struct tag *)0x30000100;

	params->hdr.tag = ATAG_CORE;	/* tag识别码 */
	params->hdr.size = tag_size(tag_core);			/* tag的大小 */
	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

void setup_memory_tags(void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size (tag_mem32);

	params->u.mem.start = 0x30000000;	/* sdram起始地址 */
	params->u.mem.size = 0x4000000;		/* sdram大小 */

	params = tag_next (params);
}

unsigned int strlen(char * str)
{
	int i = 0;
	while (str[i])
		i++;
	return i;
}

char * strcpy(char * dest, const char * src)
{
	char * tmp = dest;
	while ((*dest++ = *src++) != '\0');

	return tmp;
}

void setup_commandline_tag(char * commandline)
{
	int len = strlen (commandline) + 1;
	
	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + len + 3) >> 2;

	strcpy (params->u.cmdline.cmdline, commandline);

	params = tag_next (params);
}

void setup_end_tag(void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

int main(void)
{
	uart0_init();

	void (*theKernel)(int zero, int arch, unsigned int params);

	volatile unsigned int * p = (volatile unsigned int *)(0x30008000 + 0x100000 - 64);
	
	/* 1、将内核从FLASH读到SDRAM */
	puts("Copy kernel from nand\n\r");
	nand_read(0x60000+64, (unsigned char *)0x30008000, 0x200000);
	puthex(0x1234abcd);
	puthex(*p);

	/* 2、设置内核要用到的参数，放在内核知道的地址 */
	puts("Set boot params\n\r");
	setup_start_tag ();
	setup_memory_tags ();
	setup_commandline_tag("noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0");
	setup_end_tag ();

	/* 3、跳到内核地址处执行 */
	puts("Boot kernel\n\r");
	theKernel = (void (*)(int, int, unsigned int))0x30008000;	/* 内核在sdram中的存放地址 */
	theKernel(0, 362, 0x30000100);

	/* 如果一切顺利，将不会执行到此处 */
	puts("Error\n\r");
	return -1;
}


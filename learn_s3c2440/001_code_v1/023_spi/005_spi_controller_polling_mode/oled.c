#include "oledfont.h"
#include "s3c24xx.h"
#include "spi_controller.h"


static void OLED_write_mode(int val)
{
	if (val)
		GPGDAT |= (1<<4);
	else
		GPGDAT &= ~(1<<4);
}

static void OLED_cs(int val)
{
	if (val)
		GPFDAT |= (1<<1);
	else
		GPFDAT &= ~(1<<1);
}


static void OLED_write_cmd(unsigned char cmd)
{
	OLED_write_mode(0);	/* 发送命令 */
	OLED_cs(0);			/* select OLED */
	SPISendByte(cmd);	/* 发送数据 */
	OLED_cs(1);			/* deselect OLED */
	OLED_write_mode(1);
}

static void OLED_write_dat(unsigned char dat)
{
	OLED_write_mode(1);	/* 发送数据 */
	OLED_cs(0);			/* select OLED */
	SPISendByte(dat);	/* 发送数据 */
	OLED_cs(1);			/* deselect OLED */
	OLED_write_mode(1);
}

static void OLEDSetPageAddrMode(void)
{
	/* Set Memory Addressing Mode */
	OLED_write_cmd(0x20);
	OLED_write_cmd(0x2);
}

static void OLEDSetPos(int page, int col)
{	
	/* Set Page Start Address for Page Addressing Mode */
	OLED_write_cmd(0xb0 + page);

	/* Set Lower Column */
	OLED_write_cmd(col &0xf);

	/* Set Higher Column */
	OLED_write_cmd(0x10 + (col>>4));
}


static void OLEDShowByte(int page, int col, char c)
{
	int i;
	/* 获得字模 */
	const unsigned char * dots = oled_asc2_8x16[c - ' '];

	/* 发送上半个字，8bit */
	OLEDSetPos(page, col);
	for (i = 0; i < 8; i++)
		OLED_write_dat(dots[i]);

	/* 发送下半个字，8bit */
	OLEDSetPos(page + 1, col);
	for (i = 0; i < 8; i++)
		OLED_write_dat(dots[i + 8]);
}

void OLEDClear(void)
{
	int page, col;

	for (page = 0; page < 8; page++)
	{
		OLEDSetPos(page, 0);
		for (col = 0; col < 128; col++)
			OLED_write_dat(0);
	}
}

void OLEDPageClear(int page)
{
	int col;

	OLEDSetPos(page, 0);
	for (col = 0; col < 128; col++)
		OLED_write_dat(0);
}

void OLEDPrint(int page, int col, char * str)
{
	int i = 0;
	while (str[i])
	{
		OLEDShowByte(page, col, str[i++]);
		col += 8;
		if (col > 127)
		{
			page += 2;
			col = 0;
		}
	}
}


void OLEDInit(void)
{
	OLED_write_cmd(0xAE); /*display off*/
	OLED_write_cmd(0x00); /*set lower column address*/
	OLED_write_cmd(0x10); /*set higher column address*/
	OLED_write_cmd(0x40); /*set display start line*/
	OLED_write_cmd(0xB0); /*set page address*/
	OLED_write_cmd(0x81); /*contract control*/
	OLED_write_cmd(0x66); /*128*/
	OLED_write_cmd(0xA1); /*set segment remap*/
	OLED_write_cmd(0xA6); /*normal / reverse*/
	OLED_write_cmd(0xA8); /*multiplex ratio*/
	OLED_write_cmd(0x3F); /*duty = 1/64*/
	OLED_write_cmd(0xC8); /*Com scan direction*/
	OLED_write_cmd(0xD3); /*set display offset*/
	OLED_write_cmd(0x00);
	OLED_write_cmd(0xD5); /*set osc division*/
	OLED_write_cmd(0x80);
	OLED_write_cmd(0xD9); /*set pre-charge period*/
	OLED_write_cmd(0x1f);
	OLED_write_cmd(0xDA); /*set COM pins*/
	OLED_write_cmd(0x12);
	OLED_write_cmd(0xdb); /*set vcomh*/
	OLED_write_cmd(0x30);	
	OLED_write_cmd(0x8d); /*set charge pump enable*/
	OLED_write_cmd(0x14);

	OLEDSetPageAddrMode();
	OLEDClear();		  /* 清屏 */

	OLED_write_cmd(0xAF); /*display ON*/
}









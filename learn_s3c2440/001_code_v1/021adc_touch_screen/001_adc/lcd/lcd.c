# include "lcd.h"
static p_lcd_params p_array_lcd[LCD_NUM];
static p_lcd_params g_p_lcd_select;  /* 指向selected的参数结构体 */

/* 注册 */
int register_lcd(p_lcd_params plcd)
{
    int i;
    for (i = 0; i < LCD_NUM; i++)
    {
        if (! p_array_lcd[i])
        {
            p_array_lcd[i] = plcd;
            return i;
        }
    }
    return -1;
}

/* 选择 */
int select_lcd(char * name)
{
    int i;
    for (i = 0; i < LCD_NUM; i++)
    {
        if (p_array_lcd[i] && !strcmp(p_array_lcd[i]->name, name))
        {
            g_p_lcd_select = p_array_lcd[i];
            return i;
        }
    }
    return -1;
}

int lcd_init(void)
{
    /* 注册lcd */
    lcd_4_3_add();

    /* 注册lcd控制器 */
    s3c2440_lcd_controller_add();

    /* 选择lcd */
    select_lcd("lcd_4.3");

    /* 选择lcd控制器 */
    select_lcd_controller("s3c2440");

    /* 使用lcd参数，初始化lcd控制器 */
    lcd_controller_init(g_p_lcd_select);
}

void lcd_enable(void)
{
    lcd_controller_enable();
}

void lcd_disable(void)
{
    lcd_controller_disable();
}

void get_lcd_params(int * x, int * y, int * bpp, unsigned int * fb)
{
    *x = g_p_lcd_select->xres;
    *y = g_p_lcd_select->yres;
    *bpp = g_p_lcd_select->bpp;
    *fb = g_p_lcd_select->fb_base;
}






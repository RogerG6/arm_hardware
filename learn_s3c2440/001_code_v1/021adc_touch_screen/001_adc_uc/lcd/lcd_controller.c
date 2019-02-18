# define LCD_CONTROLLER_NUM 10
# include "lcd_controller.h"


static p_lcd_controller p_array_lcd_controller[LCD_CONTROLLER_NUM];
static p_lcd_controller g_p_lcd_controller;

/* 注册 */
int register_lcd_controller(p_lcd_controller plcdcon)
{
    int i;
    for (i = 0; i < LCD_CONTROLLER_NUM; i++)
    {
        if (! p_array_lcd_controller[i])
        {
            p_array_lcd_controller[i] = plcdcon;
            return i;
        }
    }
    return -1;
}

/* 选择 */
int select_lcd_controller(char * name)
{
    int i;
    for (i = 0; i < LCD_CONTROLLER_NUM; i++)
    {
        if (p_array_lcd_controller[i] && !strcmp(p_array_lcd_controller[i]->name, name))
        {
            g_p_lcd_controller = p_array_lcd_controller[i];
            return i;
        }
    }
    return -1;
}

int lcd_controller_init(p_lcd_params plcdparams)
{
    /* 调用所选择的LCD控制器的初始化函数 */
    if (g_p_lcd_controller)
    {
        g_p_lcd_controller->init(plcdparams);
        g_p_lcd_controller->palette_init();
        return 0;
    }
    return -1;
}

void lcd_controller_enable(void)
{
    /* 调用所选择的LCD控制器的初始化函数 */
    if (g_p_lcd_controller)
    {
        g_p_lcd_controller->enable();
    }
}
void lcd_controller_disable(void)
{
    /* 调用所选择的LCD控制器的初始化函数 */
    if (g_p_lcd_controller)
    {
        g_p_lcd_controller->disable();
    }
}








#ifndef LCD_DISPLAY_TASK_H_
#define LCD_DISPLAY_TASK_H_

enum MENUITEM
{

};

struct LcdDataStruct
{
    char line_1[17];
    char line_2[17];
};

void lcd_display_task(void *params);

#endif // LCD_DISPLAY_TASK_H_

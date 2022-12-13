#ifndef LCD_DISPLAY_TASK_H_
#define LCD_DISPLAY_TASK_H_

struct LcdStringsStruct
{
    char *line_1;
    char *line_2;
};

void lcd_display_task(void *params);

#endif // LCD_DISPLAY_TASK_H_

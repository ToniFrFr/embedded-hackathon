#ifndef LCD_DISPLAY_TASK_H_
#define LCD_DISPLAY_TASK_H_

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

#include "../menu/menu_tasks.h"

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

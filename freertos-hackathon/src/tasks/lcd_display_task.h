/**
 * @file lcd_display_task.h
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief Declaration of tasks, structs and enums related to controlling the LCD
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LCD_DISPLAY_TASK_H_
#define LCD_DISPLAY_TASK_H_

/// \cond
#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"
#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
/// \endcond

#include "../menu/menu_tasks.h"

/**
 * @brief Task that handles LCD operation
 *
 * @param params FreeRTOS task prototype param
 */
void lcd_display_task(void *params);

#endif // LCD_DISPLAY_TASK_H_

/**
 * @file lcd_display_task.cpp
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief Declaration of tasks related to controlling the LCD
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "lcd_display_task.h"

/**
 * @brief Task that handles LCD operation
 *
 * @param params FreeRTOS task prototype param
 */
void lcd_display_task(void *params)
{
    /**
     * @brief Receive queue for the values to print
     *
     */
    extern QueueHandle_t strings_to_print_queue;

    /**
     * @brief Set the LCD display pins and initialise the LCD display object
     *
     */
    DigitalIoPin *rs = new DigitalIoPin(0, 29, DigitalIoPin::output);
    DigitalIoPin *en = new DigitalIoPin(0, 9, DigitalIoPin::output);
    DigitalIoPin *d4 = new DigitalIoPin(0, 10, DigitalIoPin::output);
    DigitalIoPin *d5 = new DigitalIoPin(0, 16, DigitalIoPin::output);
    DigitalIoPin *d6 = new DigitalIoPin(1, 3, DigitalIoPin::output);
    DigitalIoPin *d7 = new DigitalIoPin(0, 0, DigitalIoPin::output);
    LiquidCrystal *lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);

    /**
     * @brief Cleat the LCD
     *
     */
    lcd->clear();

    /**
     * @brief Struct to hold the received menu values
     *
     */
    MenuValuesStruct received_struct;

    /**
     * @brief Variables to hold the formatted strings to print
     *
     */
    char string_1[17];
    char string_2[17];

    /**
     * @brief Status to signal newly available strings to print (not used)
     *
     */
    BaseType_t stringsToPrintStatus;

    while (true)
    {
        /**
         * @brief Wait for available data to print
         *
         */
        stringsToPrintStatus = xQueueReceive(strings_to_print_queue, &received_struct, portMAX_DELAY);

        /**
         * @brief If new data to print is received format the strings based on editing mode and print them to LCD
         *
         */
        if (received_struct.editing == 0)
        {
            snprintf(string_1, 17, "    Edit SP:%4d", received_struct.setpoint);
            snprintf(string_2, 17, "T:%3dC    H:%3d%", received_struct.temperature, received_struct.humidity);
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print(string_1);
            lcd->setCursor(0, 1);
            lcd->print(string_2);
        }
        else
        {
            snprintf(string_1, 17, "CO2:%4d SP:%4d", received_struct.co2, received_struct.setpoint);
            snprintf(string_2, 17, "T:%3dC    H:%3d%", received_struct.temperature, received_struct.humidity);
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print(string_1);
            lcd->setCursor(0, 1);
            lcd->print(string_2);
        }
    }
}

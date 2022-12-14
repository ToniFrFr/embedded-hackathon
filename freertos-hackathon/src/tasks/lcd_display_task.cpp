#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

#include "lcd_display_task.h"

void lcd_display_task(void *params)
{
    extern QueueHandle_t strings_to_print_queue;

    DigitalIoPin *rs = new DigitalIoPin(0, 29, DigitalIoPin::output);
    DigitalIoPin *en = new DigitalIoPin(0, 9, DigitalIoPin::output);
    DigitalIoPin *d4 = new DigitalIoPin(0, 10, DigitalIoPin::output);
    DigitalIoPin *d5 = new DigitalIoPin(0, 16, DigitalIoPin::output);
    DigitalIoPin *d6 = new DigitalIoPin(1, 3, DigitalIoPin::output);
    DigitalIoPin *d7 = new DigitalIoPin(0, 0, DigitalIoPin::output);
    LiquidCrystal *lcd = new LiquidCrystal(rs, en, d4, d5, d6, d7);

    // configure display geometry
    lcd->begin(16, 2);

    LcdStringsStruct received_struct;

    while (true)
    {
        xQueueReceive(strings_to_print_queue, &received_struct, portMAX_DELAY);

        if (received_struct.line_1[0] != '\0' || received_struct.line_2[0] != '\0')
        {
            lcd->clear();
        }

        if (received_struct.line_1[0] != '\0')
        {
            lcd->setCursor(0, 0);
            lcd->print(received_struct.line_1);
            Board_UARTPutSTR(received_struct.line_1);
            Board_UARTPutSTR("\r\n");
        }

        if (received_struct.line_2[0] != '\0')
        {
            lcd->setCursor(0, 1);
            lcd->print(received_struct.line_2);
            Board_UARTPutSTR(received_struct.line_2);
            Board_UARTPutSTR("\r\n");
        }
    }
}

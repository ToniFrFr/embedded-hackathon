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

    EditTypeSentStruct received_struct;

    char string[17];

    while (true)
    {
        xQueueReceive(strings_to_print_queue, &received_struct, portMAX_DELAY);

        lcd->clear();

        switch (received_struct.editType)
        {
        case EDIT_TYPE::SETPOINT_EDIT:
            snprintf(string, 17, "    [%6d]    ", received_struct.value);
            lcd->setCursor(0, 0);
            lcd->print("Setpoint");
            lcd->setCursor(0, 1);
            lcd->print(string);
            break;
        case EDIT_TYPE::SETPOINT_FIXED:
            snprintf(string, 17, "     %6d     ", received_struct.value);
            lcd->setCursor(0, 0);
            lcd->print("Setpoint");
            lcd->setCursor(0, 1);
            lcd->print(string);
            break;
        case EDIT_TYPE::CO2:
            snprintf(string, 17, "     %6d     ", received_struct.value);
            lcd->setCursor(0, 0);
            lcd->print("CO2");
            lcd->setCursor(0, 1);
            lcd->print(string);
            break;
        case EDIT_TYPE::TEMPERATURE:
            snprintf(string, 17, "     %6d     ", received_struct.value);
            lcd->setCursor(0, 0);
            lcd->print("Temperature");
            lcd->setCursor(0, 1);
            lcd->print(string);
            break;
        case EDIT_TYPE::HUMIDITY:
            snprintf(string, 17, "     %6d     ", received_struct.value);
            lcd->setCursor(0, 0);
            lcd->print("Humidity");
            lcd->setCursor(0, 1);
            lcd->print(string);
            break;
        default:
            break;
        }

/*         if (received_struct.line_1[0] != '\0' || received_struct.line_2[0] != '\0')
        {
        }

        if (received_struct.line_1[0] != '\0')
        {
            lcd->setCursor(0, 0);
            lcd->print(received_struct.line_1);
        }

        if (received_struct.line_2[0] != '\0')
        {
            lcd->setCursor(0, 1);
            lcd->print(received_struct.line_2);
        } */
    }
}

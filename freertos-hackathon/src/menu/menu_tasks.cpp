#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

#include "MenuItem.h"
#include "SimpleMenu.h"
#include "edits/ConstIntEdit.h"
#include "edits/IntEdit.h"
#include "../tasks/lcd_display_task.h"

#include "menu_tasks.h"

void menu_command_task(void *params)
{
    extern QueueHandle_t menu_command_queue;
    int32_t command_to_send = (int32_t)params;

    while (true)
    {
        xQueueSendToBack(menu_command_queue, &command_to_send, portMAX_DELAY);
    }
}

void menu_operate_task(void *params)
{
    extern QueueHandle_t menu_command_queue;
    extern QueueHandle_t strings_to_print_queue;

    IntEdit *co2_setpoint = new IntEdit("CO2 setpoint", 0, 2000, 1);
    ConstIntEdit *co2_measured = new ConstIntEdit("CO2");
    ConstIntEdit *humidity = new ConstIntEdit("Humidity");
    ConstIntEdit *temperature = new ConstIntEdit("Temperature");

    SimpleMenu menu;
    menu.addItem(new MenuItem(co2_setpoint));
    menu.addItem(new MenuItem(co2_measured));
    menu.addItem(new MenuItem(humidity));
    menu.addItem(new MenuItem(temperature));

    int32_t receivedCommand;
    LcdStringsStruct structToSend;

    while (true)
    {
        xQueueReceive(menu_command_queue, &receivedCommand, portMAX_DELAY);

        switch (receivedCommand)
        {
        case MENU::UP:
            structToSend = menu.event(MenuItem::up);
            break;
        case MENU::DOWN:
            structToSend = menu.event(MenuItem::down);
            break;
        case MENU::OK:
            structToSend = menu.event(MenuItem::ok);
            break;
        default:
            break;
        }

        xQueueSend(strings_to_print_queue, &structToSend, portMAX_DELAY);
    }
}

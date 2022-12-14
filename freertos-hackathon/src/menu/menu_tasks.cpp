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

void menu_operate_task(void *params)
{
    extern QueueHandle_t menu_command_queue;
    extern QueueHandle_t strings_to_print_queue;

    extern SemaphoreHandle_t modbus_data;

    IntEdit *co2_setpoint = new IntEdit("CO2 setpoint", 0, 2000, 5);
    ConstIntEdit *co2_measured = new ConstIntEdit("CO2");
    ConstIntEdit *humidity = new ConstIntEdit("Humidity");
    ConstIntEdit *temperature = new ConstIntEdit("Temperature");

    SimpleMenu menu;
    menu.addItem(new MenuItem(co2_setpoint));
    menu.addItem(new MenuItem(co2_measured));
    menu.addItem(new MenuItem(humidity));
    menu.addItem(new MenuItem(temperature));

    MenuCommandWithTicksStruct receivedCommand;
    LcdDataStruct structToSend;
    TickType_t lastTickCount = 0;

    BaseType_t semaphoreStatus;

    while (true)
    {
        xQueueReceive(menu_command_queue, &receivedCommand, portMAX_DELAY);

        semaphoreStatus = xSemaphoreTake(modbus_data, 0);
        if (semaphoreStatus == pdPASS)
        {
            // Read data
        }

        if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(40))
        {
            lastTickCount = xTaskGetTickCount();

            switch (receivedCommand.command)
            {
            case MENU::UP:
                menu.event(MenuItem::up);
                break;
            case MENU::DOWN:
                menu.event(MenuItem::down);
                break;
            case MENU::OK:
                menu.event(MenuItem::ok);
                break;
            default:
                break;
            }

            structToSend = menu.getSelectedPropertyEdit()->display();
        }

        xQueueSend(strings_to_print_queue, &structToSend, portMAX_DELAY);
    }
}

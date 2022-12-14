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
    extern QueueHandle_t sendReadSetpointQueue;
    extern QueueHandle_t sendNewSetpointToEepromQueue;

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
    LcdStringsStruct structToSend;
    TickType_t lastTickCount = 0;

    BaseType_t setpointReceiveStatus;
    uint32_t setpointAtBoot;
    uint32_t currentSetpoint;
    while (true)
    {
        setpointReceiveStatus = xQueueReceive(sendReadSetpointQueue, &setpointAtBoot, 0);
        if (setpointReceiveStatus == pdPASS)
        {
            currentSetpoint = setpointAtBoot;
            co2_setpoint->setValue(setpointAtBoot);
        }

        if (co2_setpoint->getValue() != currentSetpoint)
        {
            currentSetpoint = co2_setpoint->getValue();
            xQueueSend(sendNewSetpointToEepromQueue, &currentSetpoint, portMAX_DELAY);
        }

        xQueueReceive(menu_command_queue, &receivedCommand, portMAX_DELAY);

        if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(40))
        {
            lastTickCount = xTaskGetTickCount();

            switch (receivedCommand.command)
            {
            case MENU::UP:
                menu.event(MenuItem::up);
                structToSend = menu.getCurrentPropertyEdit()->display();
                break;
            case MENU::DOWN:
                menu.event(MenuItem::down);
                structToSend = menu.getCurrentPropertyEdit()->display();
                break;
            case MENU::OK:
                menu.event(MenuItem::ok);
                structToSend = menu.getCurrentPropertyEdit()->display();
                break;
            default:
                break;
            }
        }

        xQueueSend(strings_to_print_queue, &structToSend, portMAX_DELAY);
    }
}

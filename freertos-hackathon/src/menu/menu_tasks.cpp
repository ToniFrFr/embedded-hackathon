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
    // extern QueueHandle_t newDataAvailableQueue;

    // extern SemaphoreHandle_t modbus_data;

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

    // MeasuredDataStruct = measuredDataStruct;

    BaseType_t modbusSemaphoreStatus;
    BaseType_t menuCommandQueueStatus;

    MenuValuesStruct menuValuesStruct;
    PropertyEdit *selectedEdit;

    uint32_t setpoint = 0;
    bool editing = false;

    bool start = true;

    while (true)
    {
        // modbusSemaphoreStatus = xSemaphoreTake(modbus_data, 0);
        // if (modbusSemaphoreStatus == pdPASS)
        //{
        //     // Read data from class
        //     measuredDataStruct.co2 = 0 measuredDataStruct.temperature = 0 measuredDataStruct.humidity = 0 xQueueSend(newDataAvailableQueue, &MeasuredDataStruct, portMAX_DELAY);
        // }
        if (start == true)
        {
            start = false;
            menuValuesStruct.editing = 1;
            menuValuesStruct.co2 = 0;
            menuValuesStruct.setpoint = setpoint;
            menuValuesStruct.temperature = 0;
            menuValuesStruct.humidity = 0;
            xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
        }

        menuCommandQueueStatus = xQueueReceive(menu_command_queue, &receivedCommand, 0);

        if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(40))
        {
            lastTickCount = xTaskGetTickCount();

            if (receivedCommand.command == MENU::OK)
            {
                if (editing == true)
                {
                    menuValuesStruct.editing = 1;
                    menuValuesStruct.co2 = 0;
                    menuValuesStruct.setpoint = setpoint;
                    menuValuesStruct.temperature = 0;
                    menuValuesStruct.humidity = 0;
                    xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
                }

                editing = !editing;
            }

            if (receivedCommand.command == MENU::UP && editing == true)
            {
                if (setpoint != 2000)
                {
                    setpoint += 5;
                }

                menuValuesStruct.editing = 0;
                menuValuesStruct.co2 = 0;
                menuValuesStruct.setpoint = setpoint;
                menuValuesStruct.temperature = 0;
                menuValuesStruct.humidity = 0;
                xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
            }

            if (receivedCommand.command == MENU::DOWN && editing == true)
            {
                if (setpoint != 0)
                {
                    setpoint -= 5;
                }

                menuValuesStruct.editing = 0;
                menuValuesStruct.co2 = 0;
                menuValuesStruct.setpoint = setpoint;
                menuValuesStruct.temperature = 0;
                menuValuesStruct.humidity = 0;
                xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
            }
        }
    }
}

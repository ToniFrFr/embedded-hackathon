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
#include "../ModbusTask.h"

#include "menu_tasks.h"

void menu_operate_task(void *params)
{
    extern QueueHandle_t menu_command_queue;
    extern QueueHandle_t strings_to_print_queue;
    extern QueueHandle_t sendReadSetpointQueue;
    extern QueueHandle_t sendNewSetpointToEepromQueue;

    MenuCommandWithTicksStruct receivedCommand;
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
        if (start == true)
        {
            start = false;
            xQueueReceive(sendReadSetpointQueue, &menuValuesStruct.setpoint, portMAX_DELAY);
            setpoint = menuValuesStruct.setpoint;
            menuValuesStruct.editing = 1;
            menuValuesStruct.co2 = 0;
            menuValuesStruct.temperature = 0;
            menuValuesStruct.humidity = 0;
            xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
        }

        menuCommandQueueStatus = xQueueReceive(menu_command_queue, &receivedCommand, 0);

        if (menuCommandQueueStatus == pdPASS)
        {
            if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(50))
            {
                lastTickCount = xTaskGetTickCount();

                if (receivedCommand.command == MENU::OK)
                {
                    if (editing == true)
                    {
                        xQueueSend(sendNewSetpointToEepromQueue, &setpoint, portMAX_DELAY);
                        menuValuesStruct.editing = 1;
                        menuValuesStruct.setpoint = setpoint;
                        xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
                    }
                    else
                    {
                        menuValuesStruct.editing = 0;
                        menuValuesStruct.setpoint = setpoint;
                        xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
                    }

                    editing = !editing;
                }

                if (receivedCommand.command == MENU::UP && editing == true)
                {
                    if (setpoint != 2000)
                    {
                        setpoint += 5;
                        if (setpoint > 2000)
                        {
                            setpoint = 2000;
                        }
                    }

                    menuValuesStruct.editing = 0;
                    menuValuesStruct.setpoint = setpoint;
                    xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
                }

                if (receivedCommand.command == MENU::DOWN && editing == true)
                {
                    if (setpoint != 0)
                    {
                        if (setpoint < 5)
                        {
                            setpoint = 0;
                        }
                        else
                        {
                            setpoint -= 5;
                        }
                    }

                    menuValuesStruct.editing = 0;
                    menuValuesStruct.setpoint = setpoint;
                    xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
                }
            }
        }

        modbusSemaphoreStatus = xSemaphoreTake(modbus.data_ready, 0);
        if (modbusSemaphoreStatus == pdPASS)
        {
            if (editing == true)
            {
                menuValuesStruct.editing = 0;
            }
            else
            {
                menuValuesStruct.editing = 1;
            }
            menuValuesStruct.setpoint = setpoint;
            menuValuesStruct.co2 = modbus.getCo2();
            menuValuesStruct.temperature = modbus.getTemperature();
            menuValuesStruct.humidity = modbus.getHumidity();
            xQueueSend(strings_to_print_queue, &menuValuesStruct, portMAX_DELAY);
        }
    }
}

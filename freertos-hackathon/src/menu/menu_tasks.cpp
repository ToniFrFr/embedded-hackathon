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

    EditTypeSentStruct editTypeSentStruct;
    PropertyEdit *selectedEdit;

    while (true)
    {
        // modbusSemaphoreStatus = xSemaphoreTake(modbus_data, 0);
        // if (modbusSemaphoreStatus == pdPASS)
        //{
        //     // Read data from class
        //     measuredDataStruct.co2 = 0 measuredDataStruct.temperature = 0 measuredDataStruct.humidity = 0 xQueueSend(newDataAvailableQueue, &MeasuredDataStruct, portMAX_DELAY);
        // }

        menuCommandQueueStatus = xQueueReceive(menu_command_queue, &receivedCommand, 0);

        if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(100))
        {
            lastTickCount = xTaskGetTickCount();

            if (menuCommandQueueStatus == pdPASS)
            {
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

                selectedEdit = menu.getSelectedPropertyEdit();

                if (selectedEdit == co2_setpoint)
                {
                    if (selectedEdit->getFocus())
                    {
                        editTypeSentStruct.editType = EDIT_TYPE::SETPOINT_EDIT;
                        editTypeSentStruct.value = selectedEdit->getEditedValue();
                    }
                    else
                    {
                        editTypeSentStruct.editType = EDIT_TYPE::SETPOINT_FIXED;
                        editTypeSentStruct.value = selectedEdit->getValue();
                    }
                }
                else if (selectedEdit == co2_measured)
                {
                    editTypeSentStruct.editType = EDIT_TYPE::CO2;
                    editTypeSentStruct.value = selectedEdit->getValue();
                }
                else if (selectedEdit == temperature)
                {
                    editTypeSentStruct.editType = EDIT_TYPE::TEMPERATURE;
                    editTypeSentStruct.value = selectedEdit->getValue();
                }
                else if (selectedEdit == humidity)
                {
                    editTypeSentStruct.editType = EDIT_TYPE::HUMIDITY;
                    editTypeSentStruct.value = selectedEdit->getValue();
                }

                xQueueSend(strings_to_print_queue, &editTypeSentStruct, portMAX_DELAY);
            }
        }
    }
}

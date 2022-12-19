/**
 * @file menu_tasks.cpp
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief Definition of menu state related tasks
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

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

#include "../tasks/lcd_display_task.h"
#include "../ModbusTask.h"
#include "menu_tasks.h"

void menu_operate_task(void *params)
{
    /**
     * @brief Receiving for menu commands from interrupts
     *
     */
    extern QueueHandle_t menu_command_queue;

    /**
     * @brief Sending queue for data to print to menu
     *
     */
    extern QueueHandle_t strings_to_print_queue;

    /**
     * @brief Receiving queue for saved EEPROM setpoint
     *
     */
    extern QueueHandle_t sendReadSetpointQueue;

    /**
     * @brief Sending queue for sending new setpoint to be saved to EEPROM
     *
     */
    extern QueueHandle_t sendNewSetpointToEepromQueue;

    /**
     * @brief Status to signal available updated modbus sensor readings
     *
     */
    BaseType_t modbusSemaphoreStatus;

    /**
     * @brief Status to signal available commands in the queue
     *
     */
    BaseType_t menuCommandQueueStatus;

    /**
     * @brief Struct to hold received menu commands from interrupt
     *
     */
    MenuCommandWithTicksStruct receivedCommand;

    /**
     * @brief Local struct to hold menu data
     *
     */
    MenuValuesStruct menuValuesStruct;

    /**
     * @brief Variable to save last tick count from interrupt for debouncing (not effective)
     *
     */
    TickType_t lastTickCount = 0;

    /**
     * @brief Initial setpoint
     *
     */
    uint32_t setpoint = 0;

    /**
     * @brief Editing state
     *
     */
    bool editing = false;

    /**
     * @brief Boolean for first initialisation
     *
     */
    bool start = true;

    while (true)
    {
        /**
         * @brief On first start wait for saved setpoint to be read from EEPROM and set it as the current setpoint. Send command to update the LCD.
         *
         */
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

        /**
         * @brief Try to read for an available menu command
         *
         */
        menuCommandQueueStatus = xQueueReceive(menu_command_queue, &receivedCommand, 0);

        /**
         * @brief If a new menu command is available try a debounce (ineffective) and do the corresponding action
         *
         */
        if (menuCommandQueueStatus == pdPASS)
        {
            if ((receivedCommand.ticks - lastTickCount) > pdMS_TO_TICKS(50))
            {
                lastTickCount = xTaskGetTickCount();

                /**
                 * @brief If menu OK either exit or enter edit mode. Send updated value to LCD.
                 * If exiting edit mode send the new setpoint to be save to EEPROM.
                 *
                 */
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

                /**
                 * @brief If menu UP command and currently editing increase setpoint and send updates to LCD
                 *
                 */
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

                /**
                 * @brief If menu DOWN command and currently editing decrease setpoint and send updates to LCD
                 *
                 */
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

        /**
         * @brief Try to take semaphore for available modbus readings. If available send new measurements to LCD.
         *
         */
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

/**
 * @file main.cpp
 * @author Christopher Romano
 * @author Toni Franciskovic
 * @author Samuel Tikkanen
 * @author Mikael Wiksten
 * @brief Main for freeRTOS hackathon project
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <cr_section_macros.h>
//
#include <atomic>
//
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"
#include "eeprom.h"
#include "string.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include "ModbusTask.h"
#include "RotaryEncoder.h"

#include "tasks/lcd_display_task.h"
#include "menu/menu_tasks.h"

/**
 * TODO: delete?
 * The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware
 */
extern "C"
{
    void vConfigureTimerForRunTimeStats(void)
    {
        Chip_SCT_Init(LPC_SCTSMALL1);
        LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
        LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
    }
}
/* end runtime statictics collection */

// Global queues
QueueHandle_t menu_command_queue;
QueueHandle_t strings_to_print_queue;
QueueHandle_t sendReadSetpointQueue;
QueueHandle_t sendNewSetpointToEepromQueue;

// Command structs
MenuCommandWithTicksStruct menuCommandStruct;

// Interrupt handlers for rotary encoder.
extern "C"
{
    // ClockWise Interrupt
    void PIN_INT0_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        menuCommandStruct.command = 0;
        menuCommandStruct.ticks = xTaskGetTickCountFromISR();

        xQueueSendFromISR(menu_command_queue, (void *)&menuCommandStruct, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    // CounterClockWise Interrupt
    void PIN_INT1_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        menuCommandStruct.command = 1;
        menuCommandStruct.ticks = xTaskGetTickCountFromISR();

        xQueueSendFromISR(menu_command_queue, (void *)&menuCommandStruct, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    // Button Press Interrupt
    void PIN_INT2_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        menuCommandStruct.command = 2;
        menuCommandStruct.ticks = xTaskGetTickCountFromISR();

        xQueueSendFromISR(menu_command_queue, (void *)&menuCommandStruct, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }
}

// EEPROM Definitions

/* EEPROM address used for storage */
#define EEPROM_ADDR 0x00000100

/* Number of bytes to read/write */
#define NUM_BYTES 32

/* Read/write buffer  */
uint32_t buffer[NUM_BYTES / sizeof(uint32_t)];

TaskHandle_t taskHandleForEepromRead = NULL;

void vEEPROMwrite(void *params)
{
    (void)params;

    uint8_t ret_code;
    uint8_t msg[4];
    uint32_t setpoint_PPM;

    while (1)
    {

        // Get the setpoint, suspend afterwards
        xQueueReceive(sendNewSetpointToEepromQueue, &setpoint_PPM, portMAX_DELAY);

        // Gets rid of a write on boot
        if (setpoint_PPM > 0)
        {

            // Disable FreeRTOS scheduler
            vTaskSuspendAll();

            // uint32_t setpoint_PPM --> uint8_t array
            msg[0] = (setpoint_PPM & 0x000000ff);
            msg[1] = (setpoint_PPM & 0x0000ff00) >> 8;
            msg[2] = (setpoint_PPM & 0x00ff0000) >> 16;
            msg[3] = (setpoint_PPM & 0xff000000) >> 24;

            ret_code = Chip_EEPROM_Write(EEPROM_ADDR, msg, NUM_BYTES);

            if (ret_code == IAP_CMD_SUCCESS)
            {
                // EEPROM Write passed
            }
            else
            {
                // EEPROM Write failed
            }

            // Resume FreeRTOS scheduler
            xTaskResumeAll();

            // New setpoint setting available
            //atom_setpoint = setpoint_PPM;
            //xSemaphoreGive(new_setpoint_available);
        }
    }
}

void vEEPROMread(void *params)
{
    (void)params;

    uint8_t ret_code;
    uint8_t msg[4];
    uint32_t setpoint_PPM;

    while (1)
    {

        // Disable FreeRTOS scheduler
        vTaskSuspendAll();

        ret_code = Chip_EEPROM_Read(EEPROM_ADDR, msg, NUM_BYTES);

        if (ret_code == IAP_CMD_SUCCESS)
        {
            // EEPROM read passed
        }
        else
        {
            // EEPROM read failed
        }

        // Get value into setpoint_PPM
        setpoint_PPM = (msg[0] & 0x000000ff) | (msg[1] & 0x0000ffff) << 8 |
                       (msg[2] & 0x00ffffff) << 16 | (msg[3] & 0xffffffff) << 24;

        // Resume FreeRTOS scheduler
        xTaskResumeAll();

        // Send setpoint
        xQueueSend(sendReadSetpointQueue, &setpoint_PPM, portMAX_DELAY);

        // New setpoint setting available
        //atom_setpoint = setpoint_PPM;
        //xSemaphoreGive(new_setpoint_available);

        // Suspend task
        vTaskSuspend(taskHandleForEepromRead);
    }
}

int main(void)
{
    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, true);

    heap_monitor_setup();

    /* Generic Initialization */
    SystemCoreClockUpdate();
    Board_Init();

    menu_command_queue = xQueueCreate(1, sizeof(MenuCommandWithTicksStruct));
    strings_to_print_queue = xQueueCreate(10, sizeof(MenuValuesStruct));
    sendReadSetpointQueue = xQueueCreate(1, sizeof(uint32_t));
    sendNewSetpointToEepromQueue = xQueueCreate(5, sizeof(uint32_t));

    /* Enable SysTick Timer */
    SysTick_Config(SystemCoreClock / 10);

    /* Enable EEPROM clock and reset EEPROM controller */
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EEPROM);
    Chip_SYSCTL_PeriphReset(RESET_EEPROM);

    // Configure Interrupts
    // Initialize PININT driver
    Chip_PININT_Init(LPC_GPIO_PIN_INT);
    // Enable PININT clock
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
    // Reset the PININT block
    Chip_SYSCTL_PeriphReset(RESET_PININT);

    // Interrupt channel for ClockWise interrupts
    Chip_INMUX_PinIntSel(0, 0, 5);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));
    NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
    NVIC_EnableIRQ(PIN_INT0_IRQn);

    // Interrupt channel for CounterClockWise interrupts
    Chip_INMUX_PinIntSel(1, 0, 6);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(1));
    NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
    NVIC_EnableIRQ(PIN_INT1_IRQn);

    // Interrupt channel for ButtonPress interrupts
    Chip_INMUX_PinIntSel(2, 1, 8);
    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
    Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(2));
    NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
    NVIC_EnableIRQ(PIN_INT2_IRQn);

    // initialize RIT (= enable clocking etc.)
    Chip_RIT_Init(LPC_RITIMER);

    // set the priority level of the interrupt
    // The level must be equal or lower than the maximum priority specified in FreeRTOS config
    // Note that in a Cortex-M3 a higher number indicates lower interrupt priority
    NVIC_SetPriority(RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

    // Create the timer
    TimerHandle_t timer = xTimerCreate("Timer", 3000, pdTRUE, NULL, modbusTimer);

    // Start the timer
    xTimerStart(timer, 0);

    xTaskCreate(vEEPROMwrite, "EEPROMwriteTask",
                configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 2UL),
                (TaskHandle_t *)NULL);

    xTaskCreate(vEEPROMread, "EEPROMreadTask",
                configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 4UL),
                (TaskHandle_t *)&taskHandleForEepromRead);

    xTaskCreate(menu_operate_task, "Menu operate task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)nullptr,
                (tskIDLE_PRIORITY + 3UL),
                (TaskHandle_t *)nullptr);

    xTaskCreate(lcd_display_task, "LCD print task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)nullptr,
                (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *)nullptr);

    /* Start the scheduler */
    vTaskStartScheduler();

    return 1;
}

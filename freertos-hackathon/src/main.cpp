/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined(__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "heap_lock_monitor.h"
#include "retarget_uart.h"

#include "ModbusRegister.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"

#include "tasks/lcd_display_task.h"
#include "menu/menu_tasks.h"

// TODO: insert other definitions and declarations here

/* The following is required if runtime statistics are to be collected
 * Copy the code to the source file where other you initialize hardware */
extern "C"
{

    void vConfigureTimerForRunTimeStats(void)
    {
        Chip_SCT_Init(LPC_SCTSMALL1);
        LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
        LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
    }
}

QueueHandle_t menu_command_queue;
QueueHandle_t strings_to_print_queue;

int main(void)
{

#if defined(__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    heap_monitor_setup();

    menu_command_queue = xQueueCreate(5, sizeof(int32_t));
    strings_to_print_queue = xQueueCreate(5, sizeof(LcdStringsStruct));

    // initialize RIT (= enable clocking etc.)
    // Chip_RIT_Init(LPC_RITIMER);
    // set the priority level of the interrupt
    // The level must be equal or lower than the maximum priority specified in FreeRTOS config
    // Note that in a Cortex-M3 a higher number indicates lower interrupt priority
    // NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

    /* xTaskCreate(menu_command_task, "Menu UP task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)MENU::UP,
                (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *)nullptr);
    xTaskCreate(menu_command_task, "Menu DOWN task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)MENU::DOWN,
                (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *)nullptr);
    xTaskCreate(menu_command_task, "Menu OK task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)MENU::OK,
                (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *)nullptr); */

    xTaskCreate(menu_operate_task, "Menu operate task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)nullptr,
                (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *)nullptr);

    xTaskCreate(lcd_display_task, "LCD print task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)nullptr,
                (tskIDLE_PRIORITY + 2UL),
                (TaskHandle_t *)nullptr);

    /* Start the scheduler */
    vTaskStartScheduler();

    return 1;
}

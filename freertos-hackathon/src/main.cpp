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
#include "RotaryEncoder.h"

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
/* end runtime statictics collection */

// Interrupt Flags
static volatile bool CLOCKWISE_INTERRUPT = false;
static volatile bool COUNTER_CLOCKWISE_INTERRUPT = false;
static volatile bool BUTTON_PRESSED_INTERRUPT = false;

// Interrupt pins
static DigitalIoPin PIN_ClockWise(0, 5, DigitalIoPin::input);
static DigitalIoPin PIN_CounterClockWise(0, 6, DigitalIoPin::input);
static DigitalIoPin PIN_Button(1, 8, DigitalIoPin::pullup);

QueueHandle_t menu_command_queue;
QueueHandle_t strings_to_print_queue;

int32_t up = 0;
int32_t down = 1;
int32_t ok = 2;

// Use timestamp and struct with ticks if needed for debouncing.
extern "C"
{
    // ClockWise Interrupt
    void PIN_INT0_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        xQueueSendFromISR(menu_command_queue, (void *)&up, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    // CounterClockWise Interrupt
    void PIN_INT1_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        xQueueSendFromISR(menu_command_queue, (void *)&down, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    // Button Press Interrupt
    void PIN_INT2_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        xQueueSendFromISR(menu_command_queue, (void *)&ok, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }
}

/*
void rotaryTask(void *params)
{
    (void) params;

    DigitalIoPin *A 	= new DigitalIoPin(0, 5, DigitalIoPin::input);
    DigitalIoPin *B 	= new DigitalIoPin(0, 6, DigitalIoPin::input);
    DigitalIoPin *BTN 	= new DigitalIoPin(1, 8, DigitalIoPin::pullup);

    RotaryEncoder rotaryEncoder(A, B, BTN);

    while(1){
        rotaryEncoder.read();
    }

}
*/

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

    // initialize RIT (= enable clocking etc.)
    Chip_RIT_Init(LPC_RITIMER);
    // set the priority level of the interrupt
    // The level must be equal or lower than the maximum priority specified in FreeRTOS config
    // Note that in a Cortex-M3 a higher number indicates lower interrupt priority
    NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

    menu_command_queue = xQueueCreate(5, sizeof(int32_t));
    strings_to_print_queue = xQueueCreate(5, sizeof(LcdStringsStruct));

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
    /*
        xTaskCreate(task1, "test",
                configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL),
                (TaskHandle_t *) NULL);
    */

    /*
        xTaskCreate(rotaryTask, "rotaryTask",
                    configMINIMAL_STACK_SIZE * 8, NULL, (tskIDLE_PRIORITY + 1UL),
                    (TaskHandle_t *) NULL);
    */
    // Initialize PININT driver
    Chip_PININT_Init(LPC_GPIO_PIN_INT);
    // Enable PININT clock
    Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
    // Reset the PININT block
    Chip_SYSCTL_PeriphReset(RESET_PININT);

    // Configure Interrupts
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
    Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));
    NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
    NVIC_EnableIRQ(PIN_INT2_IRQn);

    // xTaskCreate(interruptResponseTask, "interruptTask",
    //             configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY + 1UL),
    //             (TaskHandle_t *)NULL);

    /* Start the scheduler */

    Board_UARTPutSTR("Starting Scheduler.. \r\n");

    xTaskCreate(menu_operate_task, "Menu operate task",
                configMINIMAL_STACK_SIZE * 4,
                (void *)nullptr,
                (tskIDLE_PRIORITY + 4UL),
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

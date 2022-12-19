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
#include "MQTTInterface/MQTTInterface.h"
#include "ModbusTask.h"
#include "RotaryEncoder.h"

#include "tasks/lcd_display_task.h"
#include "menu/menu_tasks.h"

#include "solenoid.h"

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
/*-----MQTT GLOBAL FUNCTIONS AND DEFINITIONS-----*/
static uint8_t ucSharedBuffer[ mqttSHARED_BUFFER_SIZE ];

static MQTTFixedBuffer_t xBuffer =
{
    .pBuffer = ucSharedBuffer,
    .size    = mqttSHARED_BUFFER_SIZE
};

uint32_t ulGlobalEntryTimeMs;
uint32_t prvGetTimeMs( void )
{
    TickType_t xTickCount = 0;
    uint32_t ulTimeMs = 0UL;

    /* Get the current tick count. */
    xTickCount = xTaskGetTickCount();

    /* Convert the ticks to milliseconds. */
    ulTimeMs = ( uint32_t ) xTickCount * MILLISECONDS_PER_TICK;

    /* Reduce ulGlobalEntryTimeMs from obtained time so as to always return the
     * elapsed time in the application. */
    ulTimeMs = ( uint32_t ) ( ulTimeMs - ulGlobalEntryTimeMs );

    return ulTimeMs;
}

/*-----MQTT GLOBAL FUNCTIONS-----*/


// Global queues
QueueHandle_t menu_command_queue;
QueueHandle_t strings_to_print_queue;
QueueHandle_t sendReadSetpointQueue;
QueueHandle_t sendNewSetpointToEepromQueue;

// Command structs
MenuCommandWithTicksStruct menuCommandStruct;

extern std::atomic<bool> valveOpen;

/* Interrupt handlers for rotary encoder. */
extern "C"
{
    /*
     * @brief Interrupt handler for ClockWise rotation
     * @param (void) : No parameters
     * @return Return nothing.
     */
    void PIN_INT0_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        menuCommandStruct.command = 0;
        menuCommandStruct.ticks = xTaskGetTickCountFromISR();

        xQueueSendFromISR(menu_command_queue, (void *)&menuCommandStruct, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    /*
     * @brief Interrupt handler for CounterClockWise rotation
     * @param (void) : No parameters
     * @return Return nothing.
     */
    void PIN_INT1_IRQHandler(void)
    {
        portBASE_TYPE xHigherPriorityWoken = pdFALSE;

        menuCommandStruct.command = 1;
        menuCommandStruct.ticks = xTaskGetTickCountFromISR();

        xQueueSendFromISR(menu_command_queue, (void *)&menuCommandStruct, &xHigherPriorityWoken);

        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));

        portEND_SWITCHING_ISR(xHigherPriorityWoken);
    }

    /*
     * @brief Interrupt handler for Button Press
     * @param (void) : No parameters
     * @return Return nothing.
     */
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

/* EEPROM Definitions */

/* EEPROM address used for storage */
#define EEPROM_ADDR 0x00000100

/* Number of bytes to read/write */
#define NUM_BYTES 32

/* Read/write buffer  */
uint32_t buffer[NUM_BYTES / sizeof(uint32_t)];

/* Atomic variable for setpoint */
std::atomic<uint32_t> atom_setpoint;

/* Global semaphore */
SemaphoreHandle_t new_setpoint_available;

/* Handle used to suspend EEPROMread task */
TaskHandle_t taskHandleForEepromRead = NULL;


/**
 * @brief FreeRTOS task to write from EEPROM
 * @param (void) *params : Task gets no parameters.
 * @return Task returns nothing.
 */
void vEEPROMwrite(void *params)
{
	(void) params;
	
	uint8_t ret_code; 		/* Used to check validity of EEPROM access */
	uint8_t msg[4];			/* Holds the data to be written to EEPROM */
	uint32_t setpoint_PPM;	/* Holds the value of the setpoint */
	
	while(1){
		/* Get the setpoint, suspend afterwards */
		xQueueReceive(sendNewSetpointToEepromQueue, &setpoint_PPM, portMAX_DELAY);
		
		/* Gets rid of a write on boot */
		if (setpoint_PPM > 0) {

			/* Disable FreeRTOS scheduler */
			vTaskSuspendAll();
			
			/* uint32_t setpoint_PPM --> uint8_t array */
			msg[0] = (setpoint_PPM & 0x000000ff);
			msg[1] = (setpoint_PPM & 0x0000ff00) >> 8;
			msg[2] = (setpoint_PPM & 0x00ff0000) >> 16;
			msg[3] = (setpoint_PPM & 0xff000000) >> 24;
			
			/* Write message to specified address */
			ret_code = Chip_EEPROM_Write(EEPROM_ADDR, msg, NUM_BYTES);
			
			if(ret_code == IAP_CMD_SUCCESS) {
				// EEPROM Write passed
			} else {
				// EEPROM Write failed
			}
			
			/* Resume FreeRTOS scheduler */
			xTaskResumeAll();
			
			/* New setpoint available, inform other interested tasks */
			atom_setpoint = setpoint_PPM;
			xSemaphoreGive(new_setpoint_available);
		}
	}
}

void vConnectionTask(void *pvParams) {
    NetworkContext_t xNetworkContext = { 0 };
    PlaintextTransportParams_t xPlaintextTransportParams = { 0 };
    MQTTContext_t xMQTTContext;
    bool methodSuccess;
    std::string publishPayload;

    xNetworkContext.pParams = &xPlaintextTransportParams;

	MQTTInterface mqttInterface(WIFI_SSID, WIFI_PASS, appconfigMQTT_BROKER_ENDPOINT, appconfigMQTT_BROKER_PORT);

	ulGlobalEntryTimeMs = prvGetTimeMs();

	for(;;) {
		methodSuccess = mqttInterface.ConnectToMQTTServer(&xNetworkContext);

		if(methodSuccess) {
			methodSuccess = mqttInterface.ConnectToMQTTBroker(&xBuffer,&xMQTTContext, &xNetworkContext);
			if(methodSuccess) {
				publishPayload = mqttInterface.GeneratePublishPayload(modbus.getCo2(), modbus.getHumidity(), modbus.getTemperature(), valveOpen, atom_setpoint);
				methodSuccess = mqttInterface.Publish(appconfigMQTT_TOPIC, publishPayload, &xMQTTContext);
			}
		}


		methodSuccess = mqttInterface.DisconnectFromMQTTServer(&xMQTTContext, &xNetworkContext);


		vTaskDelay(pdMS_TO_TICKS(appconfigMQTT_SEND_INTERVAL));

	}

}

/**
 * @brief FreeRTOS task to read from EEPROM
 * @param (void) *params : Task gets no parameters.
 * @return Task returns nothing.
 */
void vEEPROMread(void *params)
{
	(void) params;
	
	uint8_t ret_code;		/* Used to check validity of EEPROM access */
	uint8_t msg[4];			/* Holds the data which is read from EEPROM */
	uint32_t setpoint_PPM;	/* Holds the setpoint which is read from EEPROM */
	
	while(1){
		/* Disable FreeRTOS scheduler */
		vTaskSuspendAll();
		
		/* Read from EEPROM */
		ret_code = Chip_EEPROM_Read(EEPROM_ADDR, msg, NUM_BYTES);
		
		if(ret_code == IAP_CMD_SUCCESS) {
			// EEPROM read passed
		} else {
			// EEPROM read failed
		}
		
		/* Get read value into setpoint_PPM */
		setpoint_PPM = (msg[0] & 0x000000ff) |
				(msg[1] & 0x0000ffff) << 8 	|
		        (msg[2] & 0x00ffffff) << 16 |
				(msg[3] & 0xffffffff) << 24;
		
		/* Resume FreeRTOS scheduler */
		xTaskResumeAll();
		
		/* Send setpoint */
		xQueueSend(sendReadSetpointQueue, &setpoint_PPM, portMAX_DELAY);
		
		/* New setpoint setting available, inform other interested tasks */
		atom_setpoint = setpoint_PPM;
		xSemaphoreGive(new_setpoint_available);
		
		/* Suspend task */
		vTaskSuspend(taskHandleForEepromRead);
    }
}

int main(void)
{
	/* Generic Initialization */


    heap_monitor_setup();
    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, true);

    menu_command_queue = xQueueCreate(1, sizeof(MenuCommandWithTicksStruct));
    strings_to_print_queue = xQueueCreate(10, sizeof(MenuValuesStruct));
    sendReadSetpointQueue = xQueueCreate(1, sizeof(uint32_t));
    sendNewSetpointToEepromQueue = xQueueCreate(5, sizeof(uint32_t));

	new_setpoint_available = xSemaphoreCreateBinary();

	valveOpen = false;

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

    // initialize RIT (= enable clocking etc.)
    Chip_RIT_Init(LPC_RITIMER);

    // set the priority level of the interrupt
    // The level must be equal or lower than the maximum priority specified in FreeRTOS config
    // Note that in a Cortex-M3 a higher number indicates lower interrupt priority
    NVIC_SetPriority(RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

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

	xTaskCreate(vEEPROMwrite, "EEPROMwriteTask",
			configMINIMAL_STACK_SIZE,
			NULL,
			(tskIDLE_PRIORITY + 3UL),
			(TaskHandle_t *) NULL);

	xTaskCreate(vEEPROMread, "EEPROMreadTask",
			configMINIMAL_STACK_SIZE,
			NULL,
			(tskIDLE_PRIORITY + 4UL),
			(TaskHandle_t *) &taskHandleForEepromRead);

	xTaskCreate(menu_operate_task, "Menu operate task",
			configMINIMAL_STACK_SIZE,
			(void *)nullptr,
			(tskIDLE_PRIORITY + 4UL),
			(TaskHandle_t *)nullptr);

	xTaskCreate(lcd_display_task, "LCD print task",
			configMINIMAL_STACK_SIZE * 3,
			(void *)nullptr,
			(tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *)nullptr);		

	xTaskCreate(solenoid, "Solenoid control task",
			configMINIMAL_STACK_SIZE,
			(void *)nullptr,
			(tskIDLE_PRIORITY + 1UL),
			(TaskHandle_t *)nullptr);
	xTaskCreate(vConnectionTask, "vConnTask", 
			512,
			NULL,
			(tskIDLE_PRIORITY + 3UL),
			(TaskHandle_t *) NULL);

	TimerHandle_t timer = xTimerCreate("Timer", 250, pdTRUE, NULL, modbusTimer);

	// Start the timer
	xTimerStart(timer, 0);

	
	/* Start the scheduler */
	vTaskStartScheduler();

	return 1;
}

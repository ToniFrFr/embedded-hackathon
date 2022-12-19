#ifndef MENU_TASKS_H_
#define MENU_TASKS_H_

/**
 * @brief Enumeration delcaration for menu input types
 *
 */
enum MENU
{
    UP,
    DOWN,
    OK
};

/**
 * @brief Struct to hold and send to queu the menu state and the relevant data items
 *
 */
struct MenuValuesStruct
{
    /**
     * @brief Edit setpoint mode: 0 = true, 1 = false
     *
     */
    uint8_t editing;

    /**
     * @brief Measured CO2 level
     *
     */
    uint32_t co2;

    /**
     * @brief Set setpoint
     *
     */
    uint32_t setpoint;

    /**
     * @brief Measure temperature level
     *
     */
    uint32_t temperature;

    /**
     * @brief Measured humidity level
     *
     */
    uint32_t humidity;
};

/**
 * @brief Struct to hold the menu command from the interrupt and ticks at that time, and then send them to a queue
 *
 */
struct MenuCommandWithTicksStruct
{
    uint8_t command;
    TickType_t ticks;
};

/**
 * @brief Task that handles menu operation
 *
 * @param params FreeRTOS task prototype param
 */
void menu_operate_task(void *params);

#endif // MENU_TASKS_H_

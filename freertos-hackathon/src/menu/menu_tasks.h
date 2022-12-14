#ifndef MENU_TASKS_H_
#define MENU_TASKS_H_

enum MENU
{
    UP,
    DOWN,
    OK
};

struct MenuValuesStruct
{
    uint8_t editing;
    uint32_t co2;
    uint32_t setpoint;
    uint32_t temperature;
    uint32_t humidity;
};

struct MenuCommandWithTicksStruct
{
    uint8_t command;
    TickType_t ticks;
};

void menu_operate_task(void *params);

#endif // MENU_TASKS_H_

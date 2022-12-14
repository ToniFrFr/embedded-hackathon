#ifndef MENU_TASKS_H_
#define MENU_TASKS_H_

enum MENU
{
    UP,
    DOWN,
    OK
};

enum EDIT_TYPE
{
    SETPOINT_FIXED,
    SETPOINT_EDIT,
    CO2,
    TEMPERATURE,
    HUMIDITY
};

struct EditTypeSentStruct
{
    uint8_t editType;
    uint32_t value;
};

struct MenuCommandWithTicksStruct
{
    uint8_t command;
    TickType_t ticks;
};

void menu_operate_task(void *params);

#endif // MENU_TASKS_H_

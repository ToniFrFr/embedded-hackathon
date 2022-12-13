#ifndef MENU_TASKS_H_
#define MENU_TASKS_H_

enum MENU
{
    UP,
    DOWN,
    OK
};

struct MenuCommandWithTicksStruct
{
    uint8_t command;
    TickType_t ticks;
};

void menu_operate_task(void *params);

#endif // MENU_TASKS_H_

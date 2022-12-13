#ifndef MENU_TASKS_H_
#define MENU_TASKS_H_

enum MENU
{
    UP,
    DOWN,
    OK
};

void menu_command_task(void *params);

void menu_operate_task(void *params);

#endif // MENU_TASKS_H_

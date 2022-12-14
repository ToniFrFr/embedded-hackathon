/**
 * @file SimpleMenu.cpp
 * @author krl
 * @brief SimpleMenu definition
 * @version 0.1
 * @date 2016-02-03
 *
 */

#include "SimpleMenu.h"

SimpleMenu::SimpleMenu()
{
    position = 0;
}

SimpleMenu::~SimpleMenu()
{
}

void SimpleMenu::addItem(MenuItem *item)
{
    items.push_back(item);
}

void SimpleMenu::event(MenuItem::menuEvent e)
{
    if (items.size() <= 0)
        return;

    if (!items[position]->event(e))
    {
        if (e == MenuItem::up)
        {
            position++;
        }
        else if (e == MenuItem::down)
        {
            position--;
        }

        if (position < 0)
        {
            position = items.size() - 1;
        }
        if (position >= (int)items.size())
        {
            position = 0;
        }
    }
}

PropertyEdit* SimpleMenu::getSelectedPropertyEdit()
{
    return items[position]->getPropertyEdit();
}

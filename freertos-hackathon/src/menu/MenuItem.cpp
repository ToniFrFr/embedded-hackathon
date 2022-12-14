/**
 * @file MenuItem.cpp
 * @author krl
 * @brief MenuItem definition
 * @version 0.1
 * @date 2016-02-01
 *
 */

#include "../tasks/lcd_display_task.h"
#include "MenuItem.h"

MenuItem::MenuItem(PropertyEdit *property) : pe(property)
{
}

MenuItem::~MenuItem()
{
}

bool MenuItem::event(menuEvent e)
{
    bool handled = true;
    switch (e)
    {
    case ok:
        if (pe->getFocus())
        {
            pe->accept();
            pe->setFocus(false);
        }
        else
        {
            pe->setFocus(true);
        }
        break;
    case back:
        if (pe->getFocus())
        {
            pe->cancel();
            pe->setFocus(false);
        }
        else
        {
            handled = false;
        }
        break;
    case show:
        break;
    case up:
        if (pe->getFocus())
            pe->increment();
        else
            handled = false;
        break;
    case down:
        if (pe->getFocus())
            pe->decrement();
        else
            handled = false;
        break;
    }
    if (handled)
        pe->display();

    return handled;
}

PropertyEdit* MenuItem::getPropertyEdit()
{
    return this->pe;
}

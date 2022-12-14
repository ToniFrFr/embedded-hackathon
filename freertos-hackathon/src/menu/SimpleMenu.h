/**
 * @file SimpleMenu.h
 * @author krl
 * @brief SimpleMenu declaration
 * @version 0.1
 * @date 2016-02-03
 *
 */

#ifndef SIMPLE_MENU_H_
#define SIMPLE_MENU_H_

/// \cond
#include <cstring>
#include <vector>
/// \endcond

#include "MenuItem.h"

/**
 * @brief Manage menu
 *
 */
class SimpleMenu
{
public:
    /**
     * @brief Construct a new Simple Menu object
     *
     */
    SimpleMenu();

    /**
     * @brief Destroy the Simple Menu object
     *
     */
    virtual ~SimpleMenu();

    /**
     * @brief Add an item to the menu
     *
     * @param item The item to add
     */
    void addItem(MenuItem *item);

    /**
     * @brief Trigger menu event
     *
     * @param e The event to trigger
     */
    void event(MenuItem::menuEvent e);

    PropertyEdit* getSelectedPropertyEdit();

private:
    std::vector<MenuItem *> items;
    int position;
};

#endif // SIMPLE_MENU_H_

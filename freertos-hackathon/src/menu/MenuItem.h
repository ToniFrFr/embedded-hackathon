//========================================//
//      DEPRECATED - FILE NOT IN USE      //
//========================================//

/**
 * @file MenuItem.h
 * @author krl
 * @brief MenuItem declaration
 * @version 0.1
 * @date 2016-02-03
 *
 */

#ifndef MENUITEM_H_
#define MENUITEM_H_

#include "edits/PropertyEdit.h"

/**
 * @brief Manage menu items
 *
 */
class MenuItem
{
public:
    /**
     * @brief Enums for menu events
     *
     */
    enum menuEvent
    {
        up,
        down,
        ok,
        back,
        show
    };

    /**
     * @brief Construct a new Menu Item object
     *
     * @param property The property the item holds
     */
    MenuItem(PropertyEdit *property);

    /**
     * @brief Destroy the Menu Item object
     *
     */
    virtual ~MenuItem();

    /**
     * @brief Handle event on menu item
     *
     * @param e The event to handle
     * @return true
     * @return false
     */
    virtual bool event(menuEvent e);

    PropertyEdit* getPropertyEdit();

private:
    PropertyEdit *pe;
};

#endif /* MENUITEM_H_ */

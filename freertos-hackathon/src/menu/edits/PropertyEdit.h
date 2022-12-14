/**
 * @file PropertyEdit.h
 * @author krl
 * @brief Abstract class for all edits
 * @version 0.1
 * @date 2016-02-01
 *
 */

#ifndef PROPERTY_EDIT_H_
#define PROPERTY_EDIT_H_

#include "../../tasks/lcd_display_task.h"

/**
 * @brief Abstract class for all edits
 *
 */
class PropertyEdit
{
public:
    PropertyEdit(){};
    virtual ~PropertyEdit(){};
    virtual void increment() = 0;
    virtual void decrement() = 0;
    virtual void accept() = 0;
    virtual void cancel() = 0;
    virtual void setFocus(bool focus) = 0;
    virtual bool getFocus() = 0;
    virtual int getValue() = 0;
    virtual int getEditedValue() = 0;
    virtual LcdDataStruct display() = 0;
};

#endif // PROPERTY_EDIT_H_

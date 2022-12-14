/**
 * @file IntEdit.h
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief IntEdit declaration
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef INT_EDIT_H_
#define INT_EDIT_H_

/// \cond
#include <cstring>
#include <cstdio>
/// \endcond

#include "../../tasks/lcd_display_task.h"
#include "PropertyEdit.h"

/**
 * @brief Hold and display modifiable integer
 *
 */
class IntEdit : public PropertyEdit
{
public:
    /**
     * @brief Construct a new IntEdit object
     *
     * @param editTitle Title of the object to display
     * @param upperLimit Value's upper limit
     * @param lowerLimit Value's lower limit
     * @param step Increment/decrement step
     */
    IntEdit(           char * editTitle,
                 int upperLimit,
                 int lowerLimit,
                 int step);

    /**
     * @brief Destroy the IntEdit object
     *
     */
    virtual ~IntEdit();

    /**
     * @brief Increment by step
     *
     */
    void increment();

    /**
     * @brief Decrement by step
     *
     */
    void decrement();

    /**
     * @brief Public call to save
     *
     */
    void accept();

    /**
     * @brief Reset changes
     *
     */
    void cancel();

    /**
     * @brief Set the focus (for menu)
     *
     * @param focus
     */
    void setFocus(bool focus);

    /**
     * @brief Get current focus value
     *
     * @return true
     * @return false
     */
    bool getFocus();

    /**
     * @brief Display object on LCD
     *
     */
    LcdDataStruct display();

    /**
     * @brief Get current value
     *
     * @return int Value of object
     */
    int getValue();

    /**
     * @brief Set the value of object
     *
     * @param value Value to set
     */
    void setValue(int value);

private:
    /**
     * @brief Save new value and reset edit.
     *
     */
    void save();

    char * title;
    int value = 0;
    int edit = 0;
    bool focus = false;
    const int step;
    const int upperLimit;
    const int lowerLimit;
};

#endif // INT_EDIT_H_

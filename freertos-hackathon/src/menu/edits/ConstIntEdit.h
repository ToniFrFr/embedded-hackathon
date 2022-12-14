/**
 * @file ConstIntEdit.h
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief ConstIntEdit declaration
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CONST_INT_EDIT_H_
#define CONST_INT_EDIT_H_

/// \cond
#include <cstdio>
#include <cstring>
/// \endcond

#include "../../tasks/lcd_display_task.h"
#include "PropertyEdit.h"

/**
 * @brief Hold and display constant integers
 *
 * Values held by this class can only be modified by
 * setting their value manually.
 */
class ConstIntEdit : public PropertyEdit
{
public:
    /**
     * @brief Construct a new ConstIntEdit object
     *
     * @param editTitle Title of the object to display
     */
    ConstIntEdit(char *editTitle);

    /**
     * @brief Destroy the ConstIntEdit object
     *
     */
    virtual ~ConstIntEdit();

    /**
     * @brief Empty function
     *
     */
    void increment();

    /**
     * @brief Empty function
     *
     */
    void decrement();

    /**
     * @brief Empty function
     *
     */
    void accept();

    /**
     * @brief Empty function
     *
     */
    void cancel();

    /**
     * @brief Empty function
     *
     * @param focus
     */
    void setFocus(bool focus);

    /**
     * @brief Always return false
     *
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
     * @brief Empty function
     *
     */
    void save();

    char *title;
    int value = 0;
};

#endif // CONST_INT_EDIT_H_

//========================================//
//      DEPRECATED - FILE NOT IN USE      //
//========================================//

/**
 * @file IntEdit.cpp
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief IntEdit definition
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "IntEdit.h"

IntEdit::IntEdit(char *editTitle,
                 int lowerLimit,
                 int upperLimit,
                 int step)
    : title(editTitle),
      value(lowerLimit),
      step(step),
      upperLimit(upperLimit),
      lowerLimit(lowerLimit)
{
}

IntEdit::~IntEdit()
{
}

void IntEdit::increment()
{
    if (this->value % this->step != 0)
    {
        this->value -= this->value % this->step;
    }

    if (this->value + this->edit + this->step <= this->upperLimit)
    {
        this->edit += this->step;
    }
}

void IntEdit::decrement()
{
    if (this->value % this->step != 0)
    {
        this->value -= this->value % this->step;
    }

    if (this->value + this->edit - this->step >= this->lowerLimit)
    {
        this->edit -= this->step;
    }
}

void IntEdit::accept()
{
    this->save();
}

void IntEdit::cancel()
{
    this->edit = 0;
}

void IntEdit::setFocus(bool focus)
{
    this->focus = focus;
}

bool IntEdit::getFocus()
{
    return this->focus;
}

LcdDataStruct IntEdit::display()
{
    LcdDataStruct localStruct;

    char string[17];

    if (this->focus)
    {
        snprintf(string, 17, "    [%6d]    ", this->value + this->edit);
    }
    else
    {
        snprintf(string, 17, "     %6d     ", this->value);
    }

    strncpy(localStruct.line_1, this->title, sizeof(localStruct.line_1));
    strncpy(localStruct.line_2, string, sizeof(localStruct.line_2));

    return localStruct;
}

void IntEdit::save()
{
    // set current value to be same as edit value
    this->value += this->edit;
    this->edit = 0;
    // todo: save current value for example to EEPROM for permanent storage
}

int IntEdit::getValue()
{
    return this->value;
}

int IntEdit::getEditedValue()
{
    return this->value + this->edit;
}

void IntEdit::setValue(int value)
{
    if (value > this->upperLimit)
    {
        this->value = this->upperLimit;
    }
    else if (value < this->lowerLimit)
    {
        this->value = this->lowerLimit;
    }
    else
    {
        this->value = value;
    }
}

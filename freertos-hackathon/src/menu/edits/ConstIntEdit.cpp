/**
 * @file ConstIntEdit.cpp
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief ConstIntEdit definition
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ConstIntEdit.h"

ConstIntEdit::ConstIntEdit(char *editTitle)
    : title(editTitle)
{
}

ConstIntEdit::~ConstIntEdit()
{
}

void ConstIntEdit::increment()
{
}

void ConstIntEdit::decrement()
{
}

void ConstIntEdit::accept()
{
}

void ConstIntEdit::cancel()
{
}

void ConstIntEdit::setFocus(bool focus)
{
}

bool ConstIntEdit::getFocus()
{
    return false;
}

LcdDataStruct ConstIntEdit::display()
{
    LcdDataStruct localStruct;

    char string[17];
    snprintf(string, 17, "     %6d     ", this->value);

    strncpy(localStruct.line_1, this->title, sizeof(localStruct.line_1));
    strncpy(localStruct.line_2, string, sizeof(localStruct.line_2));

    return localStruct;
}

void ConstIntEdit::save()
{
}

int ConstIntEdit::getValue()
{
    return this->value;
}

int ConstIntEdit::getEditedValue()
{
}

void ConstIntEdit::setValue(int value)
{
    this->value = value;
}

/**
 * @file DebounceCounter.cpp
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief DebounceCounter definition
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "DebounceCounter.h"

DebounceCounter::DebounceCounter(uint32_t limit) : limit(limit)
{
}

DebounceCounter::~DebounceCounter()
{
}

bool DebounceCounter::checkValidPress()
{
    bool isPressValid = false;

    this->now = millis();

    if (this->now - this->old > this->limit)
    {
        this->old = this->now;
        isPressValid = true;
    }

    return isPressValid;
}

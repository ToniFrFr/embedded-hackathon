/**
 * @file DebounceCounter.h
 * @author Christopher Romano (christopher.romano@metropolia.fi)
 * @brief DebounceCounter declaration
 * @version 0.1
 * @date 2022-10-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef DEBOUNCE_COUNTER_H_
#define DEBOUNCE_COUNTER_H_

#include "../systick.h"

/**
 * @brief Simplified interfaces for when I/O pins are used as buttons
 *
 */
class DebounceCounter
{
public:
    /**
     * @brief Construct a new Debounce Counter object
     *
     * @param limit Counter time limit in ms
     */
    DebounceCounter(uint32_t limit);

    /**
     * @brief Destroy the Debounce Counter object
     *
     */
    virtual ~DebounceCounter();

    /**
     * @brief Check if the press was valid
     *
     * If the interval between detected presses is too small the presses are discarded.
     *
     * @return true
     * @return false
     */
    bool checkValidPress();

private:
    uint32_t limit;
    uint32_t old = 0;
    uint32_t now = 0;
};

#endif // DEBOUNCE_COUNTER_H_

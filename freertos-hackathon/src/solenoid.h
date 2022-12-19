/**
 * @file solenoid.h
 * @author Samuel Tikkanen
 * @brief solenoid task declaration
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SOLENOID_H_
#define SOLENOID_H_

/**
 * @brief Task for controlling the state of the solenoid, regulating it on the basis of the co2 value read from the sensors
 *
 * @param params task parameters, not used for this task
 *
 */
void solenoid(void *params);

#endif

/**
 * @file solenoid.cpp
 * @author Samuel Tikkanen
 * @brief solenoid task definition
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

/// \cond
#include <climits>
#include <atomic>
/// \endcond
#include "DigitalIoPin.h"
#include "ModbusTask.h"

#include "solenoid.h"

/**
 * @brief Global atomic state of whether the solenoid is open or not
 *
 */
std::atomic<bool> valveOpen;

/**
 * @brief Semaphore which is taken when a new setpoint should be available to be used
 *
 */
extern SemaphoreHandle_t new_setpoint_available;

/**
 * @brief Global atomic variable from which to get the current setpoint
 *
 */
extern std::atomic<uint32_t> atom_setpoint;

/**
 * @brief Task for controlling the state of the solenoid, regulating it on the basis of the co2 value read from the sensors
 *
 * @param params task parameters, not used for this task
 *
 */
void solenoid(void *params) {
	DigitalIoPin relay(0, 27, DigitalIoPin::pinMode::output);
	uint32_t setpoint;
	int co2;

	while(1) {
		xSemaphoreTake(modbus.co2_ready, portMAX_DELAY);
		BaseType_t ret = xSemaphoreTake(new_setpoint_available, 0);
		if(ret == pdPASS) {
			setpoint = atom_setpoint;
		}
		co2 = modbus.getCo2();
		if(co2 < setpoint - 25) {
			relay.write(true);
			valveOpen = true;
		} else if(co2 > setpoint + 25) {
			relay.write(false);
			valveOpen = false;
		}
	}
}

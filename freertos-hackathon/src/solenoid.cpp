/// \cond
#include <climits>
#include <atomic>
/// \endcond
#include "DigitalIoPin.h"
#include "ModbusTask.h"

#include "solenoid.h"

std::atomic<bool> valveOpen;

extern SemaphoreHandle_t new_setpoint_available;
extern std::atomic<uint32_t> atom_setpoint;

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

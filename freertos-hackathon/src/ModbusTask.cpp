/**
 * @file ModbusInterface.cpp
 * @author Samuel Tikkanen
 * @brief SensorState and modbusTimer definition
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

/// \cond
#include <climits>
#include <mutex>
/// \endcond
#include "FreeRTOS.h"
#include "timers.h"
#include "modbus/ModbusMaster.h"
#include "ModbusInterface.h"
#include "Fmutex.h"

#include "ModbusTask.h"

SensorState::SensorState(uint8_t co2DevAddr, uint8_t tempHumidDevAddr,
		uint16_t co2RegAddr, uint16_t tempRegAddr, uint16_t humidRegAddr,
		uint16_t baudRate)
	: co2Interface(co2DevAddr, baudRate), tempHumidInterface(tempHumidDevAddr, baudRate),
	co2Register(co2RegAddr), tempRegister(tempRegAddr), humidityRegister(humidRegAddr), data_ready(xSemaphoreCreateBinary()), co2_ready(xSemaphoreCreateBinary()) {}

SensorState::~SensorState() {}

int SensorState::getCo2() {
	std::lock_guard<Fmutex> lock(this->mutex);
	return this->co2;
}

int SensorState::getTemperature() {
	std::lock_guard<Fmutex> lock(this->mutex);
	return this->temperature / 10;
}

int SensorState::getHumidity() {
	std::lock_guard<Fmutex> lock(this->mutex);
	return this->humidity / 10;
}

void SensorState::readRegisters() {
	std::lock_guard<Fmutex> lock(this->mutex);
	int t_co2 = this->co2Interface.readSingleRegister(this->co2Register);
	int t_temperature = this->tempHumidInterface.readSingleRegister(this->tempRegister);
	int t_humidity = this->tempHumidInterface.readSingleRegister(this->humidityRegister);
	if(t_co2 != INT_MIN) {
		this->co2 = t_co2;
	}
	if(t_temperature != INT_MIN) {
		this->temperature = t_temperature;
	}
	if(t_humidity != INT_MIN) {
		this->humidity = t_humidity;
	}
	xSemaphoreGive(this->co2_ready);
	xSemaphoreGive(this->data_ready);
}

SensorState modbus(240, 241, 0x0100, 0x0101, 0x0100, 9600);

void modbusTimer(TimerHandle_t handle) {
	modbus.readRegisters();
}


/**
 * @file ModbusTask.h
 * @author Samuel Tikkanen
 * @brief modbusTimer and SensorState declaration
 * @version 0.1
 * @date 2022-12-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MODBUS_TASK_H_
#define MODBUS_TASK_H_

/// \cond
#include <climits>
/// \endcond
#include "FreeRTOS.h"
#include "timers.h"
#include "Fmutex.h"
#include "ModbusInterface.h"

/**
 * @brief Class for managing and accessing sensor state
 *
 */
class SensorState {
public:
    /**
     * @brief Construct a new Sensor State object
     *
     * @param co2DevAddr the modbus addresss of the co2 sensor
     * @param tempHumidDevAddr the modbus addresss of the combined temperature and humidity sensor
     * @param co2RegAddr the address of the register to read on the co2 sensor
     * @param tempRegAddr the address of the register to read to get the temperature on the combined sensor
     * @param humidRegAddr the address of the register to read to get the humidity on the combined sensor
     * @param baudRate the UART speed for the given modbus device
     */
	SensorState(uint8_t co2DevAddr, uint8_t tempHumidDevAddr, uint16_t co2RegAddr, uint16_t tempRegAddr, uint16_t humidRegAddr, uint16_t baudRate);

    /**
     * @brief Destroy a Sensor State object
     *
     */
	virtual ~SensorState();

    /**
     * @brief Get the co2 value
     *
     */
	int getCo2();

    /**
     * @brief Get the temperature value
     *
     */
	int getTemperature();

    /**
     * @brief Get the humidity value
     *
     */
	int getHumidity();

    /**
     * @brief Read the registers into the object
     *
     */
	void readRegisters();

    /**
     * @brief Semaphore for notifying that new values are available for the display tasks
     *
     */
	SemaphoreHandle_t data_ready;

    /**
     * @brief Semaphore for notifying that new values are available to the solenoid task
     *
     */
	SemaphoreHandle_t co2_ready;
private:
    /**
     * @brief Mutex for guarding access to reads when new values are being retrieved from registers
     *
     */
	Fmutex mutex;

    /**
     * @brief Object for interfacing with co2 sensor over modbus
     *
     */
	ModbusInterface co2Interface;

    /**
     * @brief Object for interfacing with temperature/humidity sensor over modbus
     *
     */
	ModbusInterface tempHumidInterface;

    /**
     * @brief Address for the co2 register
     *
     */
	const uint16_t co2Register;

    /**
     * @brief Address for the temperature register
     *
     */
	const uint16_t tempRegister;

    /**
     * @brief Address for the humidity register
     *
     */
	const uint16_t humidityRegister;

    /**
     * @brief Most recent measured value for co2
     *
     */
	int co2 = 0;

    /**
     * @brief Most recent measured value for temperature
     *
     */
	int temperature = 0;

    /**
     * @brief Most recent measured value for relative humidity
     *
     */
	int humidity = 0;
};

/**
 * @brief Global reference to modbus object to enable easy use by simply including the header
 *
 */
extern SensorState modbus;

/**
 * @brief Timer task for measuring the values of the sensors into the modbus object
 *
 * @param handle Timer handle, not used by the task
 *
 */
void modbusTimer(TimerHandle_t handle);

#endif // MODBUS_TASK_H_

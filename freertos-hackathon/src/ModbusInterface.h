/**
 * @file ModbusInterface.h
 * @author Mikael Wiksten
 * @brief ModbusInterface declaration
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef MODBUS_INTERFACE_H_
#define MODBUS_INTERFACE_H_

/// \cond
#include <climits>
#include "chip.h"
/// \endcond

#include "modbus/ModbusMaster.h"

/**
 * @brief Class to facilitate modbus transactions
 *
 */
class ModbusInterface
{
public:
    /**
     * @brief Construct a new Modbus Interface object
     *
     * @param modbusDevice the modbus device
     * @param baudRate the UART speed for the given modbus device
     */
    ModbusInterface(uint8_t modbusAddress, uint16_t baudRate);

    /**
     * @brief Destroy the Modbus Interface object
     *
     */
    virtual ~ModbusInterface();

    /**
     * @brief Interface to read a single modbus register
     *
     * @param registerAddress
     * @return int Read value
     */
    int readSingleRegister(uint16_t registerAddress);

    /**
     * @brief Interface to write to a single modbus register
     *
     * @param registerAddress
     * @param value
     */
    void writeSingleRegister(uint16_t registerAddress, uint16_t value);

private:
    ModbusMaster modbusDevice;
};

#endif // MODBUS_INTERFACE_H_

/**
 * @file ModbusInterface.cpp
 * @author Mikael Wiksten
 * @brief ModbusInterface definition
 * @version 0.1
 * @date 2022-10-17
 *
 * @copyright Copyright (c) 2022
 *
 */

/// \cond
#include <cstring>
#include <climits>
/// \endcond
#include "modbus/ModbusMaster.h"
#include "ModbusInterface.h"

ModbusInterface::ModbusInterface(uint8_t modbusAddress, uint16_t baudRate) : modbusDevice(modbusAddress)
{
    modbusDevice.begin(baudRate);
}

ModbusInterface::~ModbusInterface()
{
}

int ModbusInterface::readSingleRegister(uint16_t registerAddress)
{
    this->modbusDevice.clearResponseBuffer();

    uint8_t result = this->modbusDevice.readHoldingRegisters(registerAddress, 1);
    vTaskDelay(pdMS_TO_TICKS(5));

    if (result == this->modbusDevice.ku8MBSuccess)
    {
        int32_t value;
        uint32_t rawValue = static_cast<uint32_t>(this->modbusDevice.getResponseBuffer(0));
        memcpy(&value, &rawValue, sizeof value);
        return static_cast<int>(value);
    }
    else
    {
        return INT_MIN;
    }
}

void ModbusInterface::writeSingleRegister(uint16_t registerAddress, uint16_t value)
{
    this->modbusDevice.setTransmitBuffer(0, registerAddress);
    this->modbusDevice.setTransmitBuffer(0, value);

    this->modbusDevice.writeMultipleRegisters(0, 1);
    vTaskDelay(pdMS_TO_TICKS(5));
}

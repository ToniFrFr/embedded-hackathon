#include "SetPointMaintainTask.h"

void setPointMaintainTask(void *params)
{
    extern QueueHandle_t newSetpointQueue;

    BaseType_t queueStatus;
    uint32_t receivedSetpoint = 0;
    uint32_t currentSetpoint = 0;

    uint32_t measuredCo2 = 0;

    // 0 = falling, 1 = rising
    uint8_t co2State = 0;

    DigitalIoPin relayPin(0, 27, DigitalIoPin::output);

    while (true)
    {
        queueStatus = xQueueReceive(newSetpointQueue, &receivedSetpoint, 0);

        if (queueStatus == pdPASS)
        {
            currentSetpoint = receivedSetpoint;
        }

        if (measuredCo2 > currentSetpoint)
        {
            if (co2State == 0)
            {
            }
            else if (co2State == 1)
            {
            }
            relayPin.write(false);
            co2State = 0;
        }
        else if (measuredCo2 < currentSetpoint)
        {
            relayPin.write(true);
            co2State = 1;
        }
    }
}

#ifndef SETPOINT_MAINTAIN_TASK_H_
#define SETPOINT_MAINTAIN_TASK_H_

#include "chip.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Fmutex.h"

#include "DigitalIoPin.h"

void setPointMaintainTask(void *params);

#endif // SETPOINT_MAINTAIN_TASK_H_

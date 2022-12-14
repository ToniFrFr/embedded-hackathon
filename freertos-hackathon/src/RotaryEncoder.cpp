/*
 * RotaryEncoder.cpp
 *
 *  Created on: 13.12.2022
 *      Author: tonif
 */

#include <RotaryEncoder.h>

RotaryEncoder::RotaryEncoder(DigitalIoPin *A, DigitalIoPin *B, DigitalIoPin *BTN) {
	// TODO Auto-generated constructor stub
	A_Pin = A;
	B_Pin = B;
	BTN_Pin = BTN;
	prevStateA = 0;
	prevStateBTN = 0;
}

RotaryEncoder::~RotaryEncoder() {
	// TODO Auto-generated destructor stub
}

int RotaryEncoder::read() {
	int dir = 0; // 1 - ClockWise, 2 - CounterClockWise, 3 - ButtonPressed, 0 = nothing

	int lastStateA = prevStateA;
	int lastStateBTN = prevStateBTN;

	if (A_Pin->read() != lastStateA && A_Pin->read() == 0) {

		if(A_Pin->read() != B_Pin->read()) {
			dir = 2;
			Board_UARTPutSTR("CounterClockWise\r\n");
		} else {
			dir = 1;
			Board_UARTPutSTR("ClockWise\r\n");
		}
	}

	if(BTN_Pin->read() != lastStateBTN && BTN_Pin->read() == 0){
		// Button press detected
		dir = 3;
		Board_UARTPutSTR("Button\r\n");
	}

	return dir;
}

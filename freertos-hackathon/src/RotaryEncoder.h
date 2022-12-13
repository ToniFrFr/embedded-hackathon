/*
 * RotaryEncoder.h
 *
 *  Created on: 13.12.2022
 *      Author: tonif
 */

#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

#include <string>
#include <cstring>
#include "DigitalIoPin.h"
#include "board.h"

class RotaryEncoder {
public:
	RotaryEncoder(DigitalIoPin *A, DigitalIoPin *B, DigitalIoPin *BTN);
	virtual ~RotaryEncoder();
	int read();
private:
	DigitalIoPin *A_Pin;
	DigitalIoPin *B_Pin;
	DigitalIoPin *BTN_Pin;
	int prevStateA;
	int prevStateBTN;
};

#endif /* ROTARYENCODER_H_ */

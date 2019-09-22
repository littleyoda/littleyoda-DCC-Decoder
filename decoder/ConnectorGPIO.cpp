/*
 * ConnectorGPIOobj.cpp
 *
 *  Created on: 03.01.2018
 *      Author: sven
 */

#include "ConnectorGPIO.h"
#include "GPIO.h"

ConnectorGPIO::ConnectorGPIO(ISettings* a, Pin* gpio, uint16_t high, uint16_t low) {
	action = a;
	pin = gpio;
	highvalue = high;
	lowvalue = low;
	GPIOobj.pinMode(gpio, INPUT_PULLUP, "GPIO");
}

ConnectorGPIO::~ConnectorGPIO() {
}


void ConnectorGPIO::GPIOChange(int p, int newValue) {
	if (p != pin->getPin()) {
		return;
	}
	action->setSettings("sd", String((newValue == 0) ? lowvalue : highvalue));
}

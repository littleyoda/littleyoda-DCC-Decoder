/*
 * ConnectorGPIO.cpp
 *
 *  Created on: 03.01.2018
 *      Author: sven
 */

#include "ConnectorGPIO.h"
#include "GPIO.h"

ConnectorGPIO::ConnectorGPIO(ISettings* a, Pin* gpio) {
	action = a;
	pin = gpio;
	GPIO.pinMode(gpio, INPUT_PULLUP, "GPIO");
}

ConnectorGPIO::~ConnectorGPIO() {
}


void ConnectorGPIO::GPIOChange(int p, int newValue) {
	if (p != pin->getPin()) {
		return;
	}
	action->setSettings("sd", String(newValue));
}

/*
 * ConnectorGPIOobj.cpp
 *
 *  Created on: 03.01.2018
 *      Author: sven
 */

#include "ConnectorGPIO.h"
#include "GPIO.h"

ConnectorGPIO::ConnectorGPIO(ISettings* a, Pin* gpio, uint16_t high, uint16_t low, String v) {
	addAction(a);
	pin = gpio;
	highvalue = high;
	lowvalue = low;
	GPIOobj.pinMode(gpio, INPUT_PULLUP, "GPIO");
	setModulName("GPIO");
	setConfigDescription(gpio->toString() + ": High " + String(high) + "/Low " + String(low) + " [" + v + "]");
	var = v;
}

ConnectorGPIO::~ConnectorGPIO() {
}


void ConnectorGPIO::GPIOChange(int p, int newValue) {
	if (p != pin->getPin()) {
		return;
    }
	send(var, String((pin->considerInvert(newValue) == 0) ? lowvalue : highvalue));
}

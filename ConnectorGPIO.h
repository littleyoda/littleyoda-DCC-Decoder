/*
 * ConnectorGPIO.h
 *
 *  Created on: 03.01.2018
 *      Author: sven
 */

#ifndef CONNECTORGPIO_H_
#define CONNECTORGPIO_H_

#include "ISettings.h"
#include "Pin.h"
#include "Connectors.h"

class ConnectorGPIO : public Connectors {
public:
	ConnectorGPIO(ISettings* a, Pin* gpio);
	virtual ~ConnectorGPIO();
	virtual void GPIOChange(int pin, int newValue);
private:
	ISettings* action;
	Pin* pin;
};

#endif /* CONNECTORGPIO_H_ */

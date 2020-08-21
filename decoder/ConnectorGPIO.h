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
	ConnectorGPIO(ISettings* a, Pin* gpio, uint16_t high, uint16_t low, String v);
	virtual ~ConnectorGPIO();
	virtual void GPIOChange(int pin, int newValue);
private:
	Pin* pin;
	uint16_t highvalue;
	uint16_t lowvalue;
	String var;
};

#endif /* CONNECTORGPIO_H_ */

/*
 * PORTS.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTS_H_
#define PORTS_H_

#include <Arduino.h>
#include "DataContainer.h"
#include "DataContainerSimpleList.h"

class Ports;

struct pinInfo {
	Ports* pinController;
	uint16_t pinNumber;
	uint16_t gPinNummer;
	String name;
	String usage;
	unsigned long supportedFunctions;
};


class Ports {
public:
	Ports(LinkedList<pinInfo*>* pi, int pinOffset);
	virtual ~Ports();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
	virtual void add(String s, int pinNumber, int gPinNummer, unsigned long supportedFunctions);
	virtual void add(String s, int pinNumber,  unsigned long supportedFunctions);
	virtual void cache(bool c);
	enum F {
		DIGITAL_INPUT = 1,
		DIGITAL_OUTPUT = 2,
		SUPPORTS_PULLUP = 4,
		SUPPORTS_PULLDOWN = 8,
		UNSTABLE_AT_STARTUP = 16,
		SUPPORTS_PWM = 32,
		SUPPORTS_ADC = 64,
		SUPPORTS_DAC = 128,
		PIN_STRAPPING_AT_STARTUP = 256,
	};

protected:
	bool doCaching;
private:
	LinkedList<pinInfo*>* pi;
	int pinOffset;
};

#endif /* PORTS_H_ */

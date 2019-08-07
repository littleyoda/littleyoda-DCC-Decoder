/*
 * PORTSESP32.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTSESP32_H_
#define PORTSESP32_H_
#ifdef ESP32
#include <Arduino.h>
#include "Ports.h"

class PortsESP32 : public Ports{
public:
	PortsESP32(LinkedList<pinInfo*>* pi, int pinOffset);
	virtual ~PortsESP32();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
private:
	void addESP32Pin(int x);
};

#endif
#endif /* PORTSESP32_H_ */

/*
 * PORTSESP8266.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTSESP8266_H_
#define PORTSESP8266_H_

#ifdef ESP8266

#include <Arduino.h>
#include "Ports.h"

class PortsESP8266 : public Ports {
public:
	PortsESP8266(LinkedList<pinInfo*>* pi, int pinOffset);
	virtual ~PortsESP8266();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
};

#endif  /* ESP8266 */
#endif /* PORTSESP8266_H_ */

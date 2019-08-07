/*
 * PORTSMCP23017.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTSMCP23017_H_
#define PORTSMCP23017_H_

#include "Ports.h"
#include "Adafruit_MCP23017.h"

class PortsMCP23017 : public Ports {
public:
	PortsMCP23017(uint8_t addr, LinkedList<pinInfo*>* pi, int pinOffset);
	virtual ~PortsMCP23017();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
	virtual void cache(bool c);

private:
	Adafruit_MCP23017* mcp;
	uint16_t cachedValue;
};

#endif /* PORTSMCP23017_H_ */

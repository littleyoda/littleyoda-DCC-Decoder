/*
 * PORTSMCP23017.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "PortsMCP23017.h"
#include "Logger.h"

PortsMCP23017::PortsMCP23017(uint8_t addr, LinkedList<pinInfo*>* pi, int idx) : Ports(pi, idx) {
	Adafruit_MCP23017* m = new Adafruit_MCP23017();
	m->begin(addr);
	for (int i = 0; i < 8; i++) {
	 	add("E" + String(idx) + "A" + String(i), (idx * 100) + i,     i,     F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
	 	add("E" + String(idx) + "B" + String(i), (idx * 100) + 8 + i, 8 + i, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
	}
}

PortsMCP23017::~PortsMCP23017() {

}

void PortsMCP23017::pinMode(uint16_t pin, uint8_t mode) {
		if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Unsupported PinMode: " + String(mode) + " for pin " + String(pin));
			return;
		}
		if (mode == INPUT_PULLUP) {
			mcp->pinMode(pin, INPUT);
			mcp->pullUp(pin, HIGH);
		} else {
			mcp->pinMode(pin, mode);
		}
}

int PortsMCP23017::digitalRead(uint16_t pin) {
	if (doCaching) {
		uint16_t t = cachedValue;
		t = (t >> pin) & 1;
		return (int)t;
	} else {
		return mcp->digitalRead(pin);
	}
}


void PortsMCP23017::digitalWrite(uint16_t pin, uint8_t val) {
	mcp->digitalWrite(pin, val);
}


void PortsMCP23017::analogWrite(uint16_t pin, int val) {

}

void PortsMCP23017::servoWrite(uint16_t pin, uint8_t val) {
  }

void PortsMCP23017::cache(bool c) {
	Ports::cache(c);
	if (c) {
		cachedValue = mcp->readGPIOAB();
	}
}
  

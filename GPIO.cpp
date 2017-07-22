/*
 * GPIO.cppgpg
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#include "GPIO.h"

GPIOClass::GPIOClass() {
	len = 11;
	mcp = NULL;
}

GPIOClass::~GPIOClass() {
	// TODO Auto-generated destructor stub
}

String GPIOClass::gpio2string(int gpio) {
	for (int i = 0; i < len; i++) {
		if (portArray[i] == gpio) {
			Serial.println("pin " + String(gpio) + " to " + portMap[i]);
			return "" + portMap[i];
		}
	}
	Logger::getInstance()->addToLog("Unbekannter GPIO: " + String(gpio));
	return "Pin " + String(gpio);
}


int GPIOClass::string2gpio(const char* pin) {
	for (int i = 0; i < len; i++) {
		if (portMap[i].equals(pin)) {
			return portArray[i];
		}
	}
	Logger::getInstance()->addToLog(
			"Unbekannte PIN in Config: " + String(pin));
	return Consts::DISABLE;
}

void GPIOClass::pinMode(uint8_t pin, uint8_t mode) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(
				"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	if (pin >= 100) {
		if (mode != INPUT && mode != OUTPUT) {
			Logger::getInstance()->addToLog("Unsupported PinMode: " + String(mode) + " for pin " + String(pin));
			return;
		}
		mcp->pinMode(pin - 100, mode);
	} else {
		::pinMode(pin, mode);
	}
}
void GPIOClass::digitalWrite(uint8_t pin, uint8_t val) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(
				"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	if (pin >= 100) {
		mcp->digitalWrite(pin - 100, val);
	} else {
		::digitalWrite(pin, val);
	}
}

void GPIOClass::analogWrite(uint8_t pin, int val) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(
				"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	if (pin >= 100) {
		Logger::getInstance()->addToLog(
				"Analog Write not possible for Pin: " + String(pin));
		return;
	}
	::analogWrite(pin, val);
}

void GPIOClass::analogWriteFreq(uint32_t freq) {
	::analogWriteFreq(freq);
}


void GPIOClass::enableMCP23017(uint8_t addr) {
	mcp = new Adafruit_MCP23017();
	mcp->begin(addr);
	len = 28;
}

GPIOClass GPIO;

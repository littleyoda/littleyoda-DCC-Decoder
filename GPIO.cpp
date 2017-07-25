/*
 * GPIO.cppgpg
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#include "GPIO.h"

GPIOClass::GPIOClass() {
	mcp = NULL;

	add("D0", D0);
	add("D1", D1);
	add("D2", D2);
	add("D3", D3);
	add("D4", D4);
	add("D5", D5);
	add("D6", D6);
	add("D7", D7);
	add("D8", D8);
#ifdef D9
	add("D9", D9);
#endif
#ifdef D10
	add("D10", D10);
#endif
}

/**
 * "D4" => 14
 */
void GPIOClass::add(String s, int pinNumber) {
	pintostring[pinNumber] = s;
	stringtopin[s] = pinNumber;
}

GPIOClass::~GPIOClass() {
}

String GPIOClass::gpio2string(int gpio) {
	std::map<int, String>::iterator d = pintostring.find(gpio);
	if (d == pintostring.end()) {
		Logger::getInstance()->addToLog("Unbekannter GPIO: " + String(gpio));
		return "Pin " + String(gpio);
	}
	return d->second;
}


int GPIOClass::string2gpio(const char* pin) {
	if (pin == NULL) {
		Logger::log("PIN fehlt");
		return Consts::DISABLE;
	}
	String s = String(pin);
	std::map<String, int>::iterator d = stringtopin.find(pin);
	if (d == stringtopin.end()) {
		Logger::getInstance()->addToLog("Unbekannter Pin in Config: " + s);
		return Consts::DISABLE;
	}
	return d->second;
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

/**
 * Fügt die sprechenden PIN Bezeichner (DA0 bis DB7) für die MCP23017 hinzu
 */

void GPIOClass::enableMCP23017(uint8_t addr) {
	mcp = new Adafruit_MCP23017();
	mcp->begin(addr);
	for (int i=0 ; i < 8; i++) {
		add("DA" + String(0), 100 + i);
		add("DB" + String(0), 108 + i);
	}
}

GPIOClass GPIO;


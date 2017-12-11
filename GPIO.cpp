/*
 * GPIO.cppgpg
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#include "GPIO.h"

GPIOClass::GPIOClass() {
	mcp = NULL;

#ifdef ARDUINO_ESP8266_ESP01
	add("D0", 16);
	add("D2", 4);
	add("D3", 0);
	add("D4", 2);
	add("D5", 14);
	add("D7", 13);
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
	add("D0", D0);
	add("D1", D1);
	add("D2", D2);
	add("D3", D3);
	add("D4", D4);
	add("D5", D5);
	add("D6", D6);
	add("D7", D7);
	add("D8", D8);
#endif
#ifdef ARDUINO_ESP 8266_NODEMCU
	add("D0", D0);
	add("D1", D1);
	add("D2", D2);
	add("D3", D3);
	add("D4", D4);
	add("D5", D5);
	add("D6", D6);
	add("D7", D7);
	add("D8", D8);
	add("D9", D9);
	add("D10", D10);
#endif
	add("DISABLE", 255);
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

void GPIOClass::pinMode(uint8_t pin, uint8_t mode, String usage) {
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
	addUsage(pin, usage);
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
		add("DA" + String(i), 100 + i);
		add("DB" + String(i), 108 + i);
	}
}

GPIOClass GPIO;

void GPIOClass::addUsage(uint8_t pin, String usage) {
	Serial.println("Adding " + usage + " to " + String(pin));
	std::map<int, String>::iterator d = pinusage.find(pin);
	String value = usage;
	if (!(d == pintostring.end() || d->second == NULL || d->second.length() == 0)) {
		value = d->second + "; " + value;
	}
	pinusage[pin] = value;
}

String GPIOClass::getUsage(String sep) {
	String out = "";
	for (std::map<int, String>::iterator i = pinusage.begin (); i != pinusage.end (); i++) {
		out +=
		String((*i).first) + "/" + gpio2string((*i).first) + ": "
		+ (*i).second
		+ sep;
	}
	return out;
}



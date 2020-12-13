/*
 * Pin.cpp
 *
 *  Created on: 12.12.2017
 *      Author: sven
 */

#include "Pin.h"
#include "GPIO.h"

Pin::Pin(const char* pindefinition) {
	invert = false;
	String pindef = String(pindefinition);
	if (pindef.startsWith("!")) {
		invert = true;
		pindef = pindef.substring(1);
	}
	pin = GPIOobj.string2gpio(pindef.c_str());

}

Pin::Pin(String pindef) {
	invert = false;
	if (pindef.startsWith("!")) {
		invert = true;
		pindef = pindef.substring(1);
	}
	pin = GPIOobj.string2gpio(pindef.c_str());

}

String Pin::toString() {
	String s = "";
	if (invert) {
		s = "!";
	}
	s += GPIOobj.gpio2string(pin) + "/" + String(pin);
	return s;
}

bool Pin::isInvert() const {
	return invert;
}

uint16_t Pin::getPin() const {
	return pin;
}

uint16_t Pin::considerInvert(uint16_t v)  {
	if (isInvert()) {
		if (v == 0) {
			return 1;
		}
		else if (v == 1) {
			return 0;
		}
	}
	return v;
}

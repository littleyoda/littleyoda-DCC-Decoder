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
	pindef = String(pindefinition);
	if (pindef.startsWith("!")) {
		invert = true;
		pindef = pindef.substring(1);
	}
	pin = GPIO.string2gpio(pindef.c_str());

}

Pin::~Pin() {
}

String Pin::toString() {
	String s = "";
	if (invert) {
		s = "!";
	}
	s += pindef + "/" + String(pin);
	return s;
}

bool Pin::isInvert() const {
	return invert;
}

uint16_t Pin::getPin() const {
	return pin;
}

const String& Pin::getPindef() const {
	return pindef;
}

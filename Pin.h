/*
 * Pin.h
 *
 *  Created on: 12.12.2017
 *      Author: sven
 */

#ifndef PIN_H_
#define PIN_H_

#include <Arduino.h>

class Pin {
public:
	Pin(const char* pindefinition);
	virtual ~Pin();
	String toString();

	int getPin() const;
	bool isInvert() const;
	const String& getPindef() const;

private:
	int pin;
	String pindef;
	bool invert;
};


#endif /* PIN_H_ */

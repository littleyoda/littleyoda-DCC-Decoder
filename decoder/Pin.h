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
	Pin(String pindefinition);
	virtual ~Pin() = default;
	String toString();
	uint16_t considerInvert(uint16_t);
	uint16_t getPin() const;
	bool isInvert() const;

private:
	uint16_t pin;
	bool invert;
};


#endif /* PIN_H_ */

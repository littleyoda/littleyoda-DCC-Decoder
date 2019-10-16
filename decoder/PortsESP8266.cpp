/*
 * PORTSESP8266.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "PortsESP8266.h"
#include "Consts.h"

PortsESP8266::PortsESP8266(LinkedList<pinInfo*>* pi, int pinOffset) : Ports(pi, pinOffset) {
	add("DISABLE", Consts::DISABLE, Consts::DISABLE, 0);
	add("disable", Consts::DISABLE, Consts::DISABLE, 0);
	add("disabled", Consts::DISABLE, Consts::DISABLE, 0);

#ifdef ARDUINO_ESP8266_ESP01
	add("D0", 16,0);
	add("D2", 4,0);
	add("D3", 0,0);
	add("D4", 2,0);
	add("D5", 14,0);
	add("D7", 13,0);
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
	add("D0", D0,0);
	add("D1", D1,0);
	add("D2", D2,0);
	add("D3", D3,0);
	add("D4", D4,0);
	add("D5", D5,0);
	add("D6", D6,0);
	add("D7", D7,0);
	add("D8", D8,0);
#endif
#ifdef ARDUINO_ESP8266_NODEMCU
	add("D0", D0,0);
	add("D1", D1,0);
	add("D2", D2,0);
	add("D3", D3,0);
	add("D4", D4,0);
	add("D5", D5,0);
	add("D6", D6,0);
	add("D7", D7,0);
	add("D8", D8,0);
	add("D9", D9,0);
	add("D10", D10,0);
#endif
}

PortsESP8266::~PortsESP8266() {
	// TODO Auto-generated destructor stub
}

void PortsESP8266::pinMode(uint16_t pin, uint8_t mode) {
	::pinMode(pin, mode);
}

int PortsESP8266::digitalRead(uint16_t pin) {
	return ::digitalRead(pin);
}
void PortsESP8266::digitalWrite(uint16_t pin, uint8_t val) {
	::digitalWrite(pin, val);
}
void PortsESP8266::analogWrite(uint16_t pin, int val) {
	::analogWrite(pin, val);
}
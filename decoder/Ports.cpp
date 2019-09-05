/*
 * PORTS.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Ports.h"

Ports::Ports(LinkedList<pinInfo*>* piList, int po) {
	pinOffset = po;
	pi = piList;
}

Ports::~Ports() {
	// TODO Auto-generated destructor stub
}

/**
 * "D4", 14
 */
void Ports::add(String s, int pinNumber, int gPinNummer, unsigned long supportedFunctions) {
	pinInfo* p = new pinInfo();
	p->name = s;
	p->pinNumber = pinNumber;
	p->supportedFunctions = supportedFunctions;
	p->pinController = this;
	p->gPinNummer = pinNumber;
	p->usage = "";
	pi->add(p);
}

void Ports::add(String s, int pinNumber, unsigned long supportedFunctions) {
	add(s, pinNumber, pinNumber + pinOffset, supportedFunctions);
}

void Ports::cache(bool c) {
	doCaching = c;
}


void Ports::pinMode(uint16_t pin, uint8_t mode) {

}
int Ports::digitalRead(uint16_t pin) {
	return -1;
}
void Ports::digitalWrite(uint16_t pin, uint8_t val) {

}
void Ports::analogWrite(uint16_t pin, int val) {
	
}

/*
 * Connectors.cpp
 *
 *  Created on: 18.07.2017
 *      Author: sven
 */

#include "Connectors.h"

#include "Connectors.h"
#include <Arduino.h>

Connectors::Connectors() {
}

Connectors::~Connectors() {
}



void Connectors::TurnoutCmd(int id, int direction, int source) {
}

void Connectors::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
}

void Connectors::DCCFunc(int id, int bit, int newvalue, int source) {
}

void Connectors::DCCFunc(int id, unsigned long int newvalue, int source) {
}


LinkedList<Connectors::requestInfo*>* Connectors::getRequestList() {
	return &requestList;
}


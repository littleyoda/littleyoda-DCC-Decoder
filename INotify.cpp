/*
 * ActionBase.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "INotify.h"

INotify::INotify() {
}

INotify::~INotify() {
}


void INotify::TurnoutCmd(int id, int direction, int source) {
}

void INotify::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
}

void INotify::DCCFunc(int id, int bit, int newvalue, int source) {
}

void INotify::DCCFunc(int id, unsigned long int newvalue, int source) {
}

void INotify::GPIOChange(int pin, int newValue) {
}

LinkedList<INotify::requestInfo*>* INotify::getRequestList() {
	return &requestList;
}



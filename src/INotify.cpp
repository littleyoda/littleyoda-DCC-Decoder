/*
 * ActionBase.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "INotify.h"

INotify::INotify() {
	r = NULL;
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


void INotify::getRequestList(LinkedList<requestInfo*>* list) {
	if (!requestListContains(list, r)) {
		list->add(r);
	}
}


boolean INotify::requestListContains(LinkedList<requestInfo*>* list, requestInfo* element) {
	if (r == NULL) {
		return true;
	}
	for (int idx = 0; idx < list->size(); idx++) {
		INotify::requestInfo* ri = list->get(idx);
		if ((ri->art == element->art) && (ri->id == element->id)) {
			return true;
		}
	}
	return false;
}


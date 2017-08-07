/*
 * ConnectorFunc2Value.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ConnectorFunc2Value.h"
#include "Utils.h"
#include "Arduino.h"

ConnectorFunc2Value::ConnectorFunc2Value(ISettings* a, int locoaddr, int *array, int len) {
	this->array = array;
	addr = locoaddr;
	action = a;
	arraylength = len;

	requestInfo* r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = addr;
	getRequestList()->add(r);
}

ConnectorFunc2Value::~ConnectorFunc2Value() {
}

void ConnectorFunc2Value::DCCFunc(int id, unsigned long int newvalue, int source) {
	if (id != this->addr) {
		return;
	}
	int out = 0;
	for (int i = 0; i < arraylength; i = i + 2) {
		if (bit_is_set(newvalue, array[i])) {
			if (out < array[i + 1]) {
				out = array[i + 1];
			}
		}
	}
	Serial.println("Value " + String(out));
	action->setSettings("sd", String(out));
}

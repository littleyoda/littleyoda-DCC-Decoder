/*
 * ConnectorTurnout2Value.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ConnectorTurnout2Value.h"
#include "Utils.h"
#include "Arduino.h"

ConnectorTurnout2Value::ConnectorTurnout2Value(ISettings* a, int turnoutAddr, int *array, int len) {
	this->array = array;
	addr = turnoutAddr;
	action = a;
	arraylength = len;

	r = new requestInfo();
	r->art = requestInfo::ART::TURNOUT;
	r->id = addr;
}

ConnectorTurnout2Value::~ConnectorTurnout2Value() {
}


void ConnectorTurnout2Value::TurnoutCmd(int id, int direction, int source) {
	if (id != this->addr) {
		return;
	}
	int out = 0;
	for (int i = 0; i < arraylength; i = i + 2) {
		if (array[i] == direction) {
			out =  array[i + 1];
		}
	}
	Serial.println("Turnout2Value. Direction: " + String(direction) + " => Value: " + String(out));
	action->setSettings("sd", String(out));
}

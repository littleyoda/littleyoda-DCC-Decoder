/*
 * ConnectorFunc2Value.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ConnectorFunc2Value.h"
#include "Utils.h"
#include "Logger.h"
#include "Arduino.h"

ConnectorFunc2Value::ConnectorFunc2Value(ISettings* a, int locoaddr, int *array, int len, String var) {
	this->array = array;
	this->var = var;
	addr = locoaddr;
	addAction(a);
	arraylength = len;

	r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = addr;

	setModulName("Func2Value");
	String out = "";
	for (int i = 0; i < arraylength; i = i + 2) {
		out += "F" + String(i) + ": " + String(i+1) + " ";
	}
	setConfigDescription(out);
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
	Logger::log(LogLevel::TRACE, "Func2Value Value " + String(out));
	send(var, String(out));
}

/*
 * ConnectorONOFF.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ConnectorONOFF.h"

ConnectorONOFF::ConnectorONOFF(ISettings* a, int locoaddr, int f) {
	action = a;
	addr = locoaddr;
	funcbit = f;

	requestInfo* r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = addr;
	requestList.add(r);

}

ConnectorONOFF::~ConnectorONOFF() {
}

void ConnectorONOFF::DCCFunc(int id, int bit, int newvalue, int source) {
	if (id == this->addr && bit == this->funcbit) {
		action->setSettings("onoff", String(newvalue));
	}
}

/*
 * ACTIONTRAKTION.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "ActionTraktion.h"
#include "Controller.h"

ActionTraktion::ActionTraktion(Controller* c, int n, int t, int v, int f) {
	normalAddr = n;
	normalTraktion = t;
	func = f;
	virtuelAddr = v;
	isNormal = true;
	controller = c;

	r = new requestInfo();
	r->art = requestInfo::LOCO;
	r->id = n;

	r2 = new requestInfo();
	r2->art = requestInfo::LOCO;
	r2->id = t;

}

ActionTraktion::~ActionTraktion() {
}


void ActionTraktion::DCCSpeed(int id, int speed, int direction, int source) {
	if (id == normalAddr && isNormal) {
		controller->notifyDCCSpeed(virtuelAddr, speed, direction, source);
	}
	if (id == normalTraktion && !isNormal) {
		controller->notifyDCCSpeed(virtuelAddr, speed, direction, source);
	}
}

void ActionTraktion::DCCFunc(int id, int bit, int newvalue, int source) {
	if (id == normalTraktion && bit == func) {
		isNormal = (newvalue == 0);
	}
	if (id == normalAddr) {
		controller->notifyDCCFun(virtuelAddr, bit, newvalue, source);
	}
}

void ActionTraktion::getRequestList(LinkedList<requestInfo*>* list) {
	if (!requestListContains(list, r)) {
		list->add(r);
	}
	if (!requestListContains(list, r2)) {
		list->add(r2);
	}

}

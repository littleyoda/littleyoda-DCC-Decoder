/*
 * ConnectorLocoSpeed.cpp
 *
 *  Created on: 18.07.2017
 *      Author: sven
 */

#include "ConnectorLocoSpeed.h"
#include "Consts.h"

ConnectorLocoSpeed::ConnectorLocoSpeed(ISettings* b, int locoaddr) {
	action = b;
	addr = locoaddr;
	requestInfo* r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = addr;
	requestList.add(r);
}

ConnectorLocoSpeed::~ConnectorLocoSpeed() {
}

void ConnectorLocoSpeed::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	if (id == addr|| id == Consts::LOCID_ALL) {
		if (speed == Consts::SPEED_EMERGENCY || speed == Consts::SPEED_STOP) {
			speed = 0;
		}
		action->setSettings("sd", String(direction * 100 * speed / SpeedSteps));
	}
}

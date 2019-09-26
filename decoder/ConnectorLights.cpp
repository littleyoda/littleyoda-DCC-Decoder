/*
 * ConnectorLights.cpp
 *
 *  Created on: 23.07.2017
 *      Author: sven
 */

#include "Utils.h"
#include "ConnectorLights.h"
#include "Consts.h"

ConnectorLights::ConnectorLights(ISettings* x, int locoaddr, int fkey, int richtung) {

	addAction(x);
	direction = richtung;
	addr = locoaddr;
	func = fkey;

	currentDirection = Consts::SPEED_FORWARD;
	currentFStatus = 0;

	r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = locoaddr;

}

ConnectorLights::~ConnectorLights() {
}

void ConnectorLights::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	currentDirection = direction;
	update();
}

void ConnectorLights::DCCFunc(int id, unsigned long int newvalue, int source) {
	if (id != this->addr) {
		return;
	}
	currentFStatus = newvalue;
	update();
}


void ConnectorLights::update() {
	int status = (currentDirection == direction && bit_is_set(currentFStatus, func)) ? 1 : 0;
	send("status", String(status));
}

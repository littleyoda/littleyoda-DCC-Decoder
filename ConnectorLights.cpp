/*
 * ConnectorLights.cpp
 *
 *  Created on: 23.07.2017
 *      Author: sven
 */

#include "Utils.h"
#include "ConnectorLights.h"
#include "Consts.h"

ConnectorLights::ConnectorLights(ISettings* vorne, ISettings* hinten, int locoaddr, int anaus) {

	ptr[0] = vorne;
	ptr[1] = hinten;
	addr = locoaddr;
	richtung = Consts::SPEED_FORWARD;

	fstatus = 0;
	f0 = anaus;

	r = new requestInfo();
	r->art = requestInfo::ART::LOCO;
	r->id = locoaddr;

}

ConnectorLights::~ConnectorLights() {
}

void ConnectorLights::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	richtung = direction;
	update();
}

void ConnectorLights::DCCFunc(int id, unsigned long int newvalue, int source) {
	if (id != this->addr) {
		return;
	}
	fstatus = newvalue;
	update();
}


void ConnectorLights::update() {
	int vorne = (richtung == Consts::SPEED_FORWARD && bit_is_set(fstatus, f0)) ? 1 : 0;
	int hinten = (richtung == Consts::SPEED_REVERSE && bit_is_set(fstatus, f0)) ? 1 : 0;
	Serial.println("LED: " + String(vorne) + " " + String(hinten));
	ptr[0]->setSettings("led", String(vorne));
	ptr[1]->setSettings("led", String(hinten));
}

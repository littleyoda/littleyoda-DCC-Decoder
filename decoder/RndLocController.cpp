/*
 * RNDLOCCONTROLLER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "RndLocController.h"

RndLocController::RndLocController(ISettings* a) {
	addAction(a);
	setModulName("Random Commands");
}

RndLocController::~RndLocController() {
	// TODO Auto-generated destructor stub
}

int RndLocController::loop() {
	send("setSpeed",String(speed));
	speed = speed + dir;
	if (speed == 127) {
		dir = -1;
	} else if (speed == 2) {
		dir = 1;
	}
	return 1000;
}
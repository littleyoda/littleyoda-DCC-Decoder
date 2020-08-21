/*
 * LOCDATACONTROLLER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "LocDataController.h"
#include "Controller.h"
#include "Consts.h"

LocDataController::LocDataController(Controller* c) {
    controller = c;
	requestLocData(1);
	setModulName("Loc-Controll");
	setConfigDescription("");
}


void LocDataController::requestLocData(int id) {
	currentID = id;
	locdata = controller->getLocData(id);
}

LocDataController::~LocDataController() {

}


void LocDataController::setSettings(String key, String value) {
    Serial.println(key + "=>" + value);
    if (key.equalsIgnoreCase("chlocid")) {
        currentID = currentID + value.toInt();
        if (currentID < 1) {
            currentID = 1;
        }
    	requestLocData(currentID);
        return;
    }
    boolean changed = false;
    int16_t speed = locdata->speed;
    int8_t dir = locdata->direction;
    if (key.equals("relSpeed")) {
        speed = speed + value.toInt();
        if (speed > 127) {
            speed = 127;
        }
        if (speed < 0) {
            speed = 0;
        }
        changed = true;
    } else if (key.equals("toggleDir") && value == "1") {
        speed = 0;
        dir = -dir;
        changed = true;
    }
    if (changed) {
        controller->sendDCCSpeed(currentID, speed, dir, Consts::SOURCE_RCKP);
    }

}


void LocDataController::getInternalStatus(IInternalStatusCallback* cb, String key) {
	if (key.equals("*") || key.equals("addr")) {
		cb->send(getName(), "addr", String(currentID));
	}
	if (key.equals("*") || key.equals("status")) {
		cb->send(getName(), "status", String(currentID) + " " + String(locdata->speed));
	}
	if (key.equals("*") || key.equals("speed")) {
        cb->send(getName(), "speed", String(locdata->speed));
	}
	if (key.equals("*") || key.equals("direction")) {
        if (locdata->direction == Consts::SPEED_FORWARD) {
            cb->send(getName(), "direction", ">");
        } else if (locdata->direction == Consts::SPEED_REVERSE) { 
            cb->send(getName(), "direction", "<");
        } else {
            cb->send(getName(), "direction", " ");
        }
	}
}

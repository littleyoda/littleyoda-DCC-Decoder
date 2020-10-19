/*
 * LOCDATACONTROLLER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "LocDataController.h"
#include "Controller.h"
#include "Consts.h"
#include "Utils.h"

LocDataController::LocDataController(Controller* c, LinkedList<int> *list) {
    controller = c;
	setModulName("Loc-Controll");
	setConfigDescription("");
    addrlist = list;
    currentIdx = 0;
    requestLocData();
}


void LocDataController::requestLocData() {
    if (addrlist->size() > 0) {
        if (currentIdx < 0) {
            currentIdx = addrlist->size() - 1;
        }
        if (currentIdx >= addrlist->size()) {
            currentIdx = 0;

        }
        currentADDR = addrlist->get(currentIdx);
    } else {
        if (currentIdx < 1) {
            currentIdx = 1;
        }
        currentADDR = currentIdx;
    }
    locdata = controller->getLocData(currentADDR);
}

LocDataController::~LocDataController() {

}


void LocDataController::setSettings(String key, String value) {
    Serial.println(key + "=>" + value);
    if (key.equalsIgnoreCase("chlocid")) {
        currentIdx = currentIdx + value.toInt();
    	requestLocData();
        return;
    }
    if (key.equalsIgnoreCase("locid+") && value == "1") {
        currentIdx = currentIdx + 1;
    	requestLocData();
        return;
    }
    if (key.equalsIgnoreCase("locid-") && value == "1") {
        currentIdx = currentIdx - 1;
    	requestLocData();
        return;
    }
    if (key.startsWith("toggleF") && value == "1") {
        int bit = key.substring(7).toInt();
        if (bit_is_set(locdata->status, bit)) {
            controller->sendDCCFun(currentADDR, bit, 0,Consts::SOURCE_RCKP);
        } else {
            controller->sendDCCFun(currentADDR, bit, 1,Consts::SOURCE_RCKP);
        }
        return;
    }
    boolean changed = false;
    int16_t speed = locdata->speed;
    int8_t dir = locdata->direction;
    if (key.equalsIgnoreCase("relSpeed")) {
        speed = speed + value.toInt();
        if (speed > 127) {
            speed = 127;
        }
        if (speed < 0) {
            speed = 0;
        }
        changed = true;
    } else if (key.equalsIgnoreCase("toggleDir") && value == "1") {
        speed = 0;
        dir = -dir;
        changed = true;
    }
    if (changed) {
        controller->sendDCCSpeed(currentADDR, speed, dir, Consts::SOURCE_RCKP);
    }

}


void LocDataController::getInternalStatus(IInternalStatusCallback* cb, String key) {
	if (key.equals("*") || key.equals("addr")) {
		cb->send(getName(), "addr", String(currentADDR));
	}
	if (key.equals("*") || key.equals("status")) {
		cb->send(getName(), "status", String(currentADDR) + " " + String(locdata->speed));
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

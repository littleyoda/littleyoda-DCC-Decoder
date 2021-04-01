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
#include "SpeedKonverter.h"

	
LocDataController::LocDataController(Controller* c, LinkedList<int> *loclist, LinkedList<int> *tolist) {
    controller = c;
	setModulName("Loc-Controll");
	setConfigDescription("");
    addrlist = loclist;
    turnoutaddrlist = tolist;
    currentIdx = 0;
    currentTurnOutIdx = 0;
    requestLocData();
    requestTurnoutData();
    for (int i = 0; i < loclist->size(); i++) {
	    requestInfo* r = new requestInfo();
	    r->art = requestInfo::LOCO;
	    r->id = loclist->get(i);
        list.add(r);
    }
    for (int i = 0; i < tolist->size(); i++) {
	    requestInfo* r = new requestInfo();
	    r->art = requestInfo::TURNOUT;
	    r->id = tolist->get(i);
        list.add(r);
    }
}

void LocDataController::requestTurnoutData() {
    if (turnoutaddrlist->size() > 0) {
        if (currentTurnOutIdx < 0) {
            currentTurnOutIdx = turnoutaddrlist->size() - 1;
        }
        if (currentTurnOutIdx >= turnoutaddrlist->size()) {
            currentTurnOutIdx = 0;

        }
        currentTurnOutAddr = turnoutaddrlist->get(currentTurnOutIdx);
    } else {
        if (currentTurnOutIdx < 1) {
            currentTurnOutIdx = 1;
        }
        currentTurnOutAddr = currentTurnOutIdx;
    }
    turnoutdata = controller->getTurnOutData(currentTurnOutAddr);

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
    if (key.equalsIgnoreCase("setlocid")) {
        currentIdx = value.toInt();
    	requestLocData();
        return;
    }
    if (key.equalsIgnoreCase("locid+")) {
        if (value == "1") {
        currentIdx = currentIdx + 1;
    	requestLocData();
        }
        return;
    }
    if (key.equalsIgnoreCase("locid-")) {
        if (value == "1") {
        currentIdx = currentIdx - 1;
    	requestLocData();
        }
        return;
    }

    if (key.equalsIgnoreCase("chturnoutid")) {
        currentTurnOutIdx = currentTurnOutIdx + value.toInt();
    	requestTurnoutData();
        return;
    }
    if (key.equalsIgnoreCase("setturnoutid")) {
        currentTurnOutIdx = value.toInt();
    	requestTurnoutData();
        return;
    }
    if (key.equalsIgnoreCase("turnoutid+")) {
        if (value == "1") {
            currentTurnOutIdx = currentTurnOutIdx + 1;
    	    requestTurnoutData();
        }
        return;
    }
    if (key.equalsIgnoreCase("turnoutid-")) {
         if (value == "1") {
            currentTurnOutIdx = currentTurnOutIdx - 1;
    	    requestTurnoutData();
         }
        return;
    }
    if (key.equalsIgnoreCase("toggleEmergencyStop")) {
        if (value == "1") {
            controller->emergencyStop(Consts::SOURCE_LOCCONTROLLER, !controller->isEmergency());
        }
        return;
    }
    if (key.equalsIgnoreCase("enableEmergencyStop")) {
        if (value == "1") {
            controller->emergencyStop(Consts::SOURCE_LOCCONTROLLER, true);
        }
        return;
    }
    if (key.equalsIgnoreCase("disableEmergencyStop")) {
        if (value == "1") {
            controller->emergencyStop(Consts::SOURCE_LOCCONTROLLER, false);
        }
        return;
    }
    if (key.startsWith("toggleF")) {

        if (value == "1") {
        int bit = key.substring(7).toInt();
        Logger::log(LogLevel::INFO, "LCNT", "ToggleF parsed: " + String(bit) + "/" + String(key.substring(7).toInt()));
        if (bit_is_set(locdata->status, bit)) {
            controller->sendDCCFun(currentADDR, bit, 0,Consts::SOURCE_RCKP);
        } else {
            controller->sendDCCFun(currentADDR, bit, 1,Consts::SOURCE_RCKP);
        }
        }
        return;
    }
    if (key.startsWith("setF")) {
        int bit = key.substring(4).toInt();
        Logger::log(LogLevel::INFO, "LCNT", "SetF set F" + String(bit) + ": " + String(value.toInt()));
        controller->sendDCCFun(currentADDR, bit, value.toInt(),Consts::SOURCE_RCKP);
        return;
    }

    boolean changed = false;
    if (key.equalsIgnoreCase("relSpeed") || key.equalsIgnoreCase("setSpeed") || key.equalsIgnoreCase("toggleDir")) {
            int16_t speed = locdata->speed;
            int8_t dir = locdata->direction;
            if (key.equalsIgnoreCase("relSpeed")) {
                speed = speed + value.toInt();
                if (speed > 127) {
                    speed = 127;
                }
                if (speed < Consts::SPEED_STOP) {
                    speed = Consts::SPEED_STOP;
                }
                changed = true;
            } else if (key.equalsIgnoreCase("setSpeed")) {
                speed = value.toInt();
                if (speed > 127) {
                    speed = 127;
                }
                if (speed < Consts::SPEED_STOP) {
                    speed = Consts::SPEED_STOP;
                }
                changed = true;
            } else if (key.equalsIgnoreCase("toggleDir") && value == "1") {
                speed = Consts::SPEED_STOP;
                dir = -dir;
                changed = true;
            }
            if (changed) {
                controller->sendDCCSpeed(currentADDR, speed, dir, Consts::SOURCE_RCKP);
            }
            return;
    }
    if (key.equalsIgnoreCase("toggleTO") || key.equalsIgnoreCase("toggleTurnOut")) {
        if ((key.equalsIgnoreCase("toggleTO") || key.equalsIgnoreCase("toggleTurnOut")) && value == "1") {
            if (turnoutdata->direction == 0) {
                turnoutdata->direction = 1;
            } else {
                turnoutdata->direction = 0;
            }
            changed = true;
        }
        if (changed) {
            controller->sendSetTurnout(String(currentTurnOutAddr), String(turnoutdata->direction));
        }
        return;
    }
    Logger::log(LogLevel::ERROR,"Unbekannter Befehl für LocDataController: " + key);
}


void LocDataController::getInternalStatus(IInternalStatusCallback* cb, String key) {
	if (key.equals("*") || key.equals("addr")) {
		cb->send(getName(), "addr", String(currentADDR));
	}
	if (key.equals("*") || key.equals("status")) {
		cb->send(getName(), "status", String(currentADDR) + " " + String(locdata->speed));
	}
	if (key.equals("*") || key.equals("fstatus")) {
        String out = "";
        for (unsigned int idx = 0; idx < 29; idx++) {
            if (bit_is_set(locdata->status, idx)) {
                if (!out.isEmpty()) {
                    out += " ";
                }
                out += "F" + String(idx);
            }
        }
		cb->send(getName(), "fstatus", out);
    }
    for (unsigned int idx = 0; idx < 29; idx++) {
        String x = "f" + String(idx);
        if (key.equals("*") || key.equals(x)) {
            if (bit_is_set(locdata->status, idx)) {
                cb->send(getName(), x, "1");
            } else { 
                cb->send(getName(), x, "0");
            }
        }
    }
	if (key.equals("*") || key.equals("speed")) {
        cb->send(getName(), "speed", SpeedKonverter::fromInternal(locdata->speed));
	}
	if (key.equals("*") || key.equals("steps")) {
        cb->send(getName(), "steps", String(locdata->speedsteps));
	}
	if (key.equals("*") || key.equals("internalspeed")) {
        cb->send(getName(), "internalspeed", String(locdata->speed));
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
	if (key.equals("*") || key.equals("turnoutaddr")) {
		cb->send(getName(), "turnoutaddr", String(currentTurnOutAddr));
	}
	if (key.equals("*") || key.equals("turnoutdirection")) {
        if (turnoutdata->direction == 0) {
            cb->send(getName(), "turnoutdirection", ">");
        } else  if (turnoutdata->direction == 1) { 
            cb->send(getName(), "turnoutdirection", "<");
        } else {
            cb->send(getName(), "turnoutdirection", " ");
        }
	}

	if (key.equals("*") || key.equals("isEmergency")) {
        cb->send(getName(), "isEmergency", String(controller->isEmergency()));
	}
}

void LocDataController::getRequestList(LinkedList<requestInfo*>* list) {
	for (int i = 0; i < list->size(); i++) {
        if (!requestListContains(list, r)) {
		    list->add(r);
	    }
	}

}

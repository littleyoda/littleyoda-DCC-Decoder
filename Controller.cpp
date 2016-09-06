/*
 * ActionReceiverMediator.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include<Arduino.h>
#include "Controller.h"
#include "Utils.h"
#include "Logger.h"

Controller::Controller() {

}

Controller::~Controller() {
}

void Controller::registerCmdReceiver(CmdReceiverBase* base) {
	receiver.add(base);
}

void Controller::doLoops() {
	int idx;
	for (idx = 0; idx < receiver.size(); idx++) {
		receiver.get(idx)->loop();
	}
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->loop();
	}
	for (idx = 0; idx < loops.size(); idx++) {
		loops.get(idx)->loop();
	}
}

void Controller::registerAction(ActionBase* base) {
	actions.add(base);
}

void Controller::registerLoop(interfaceLoop* loop) {
	loops.add(loop);
}

void Controller::notifyTurnout(int id, int direction) {
	// ignore the same command within 50 msec
	if (lastTurnoutCmd[0] == id && lastTurnoutCmd[1] == direction
			&& (millis() - lastTurnoutCmd[2]) < 50) {
		lastTurnoutCmd[2] = millis();
		return;
	}
	Logger::getInstance()->addToLog(
			"Turnout-CMD [ID:" + String(id) + "/ D:" + String(direction) + "]");
	lastTurnoutCmd[0] = id;
	lastTurnoutCmd[1] = direction;
	lastTurnoutCmd[2] = millis();

	// Send the information to the actions
	int idx;
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->TurnoutCmd(id, direction);
	}
}

String Controller::getHTMLController() {
	String msg = "";
	for (int idx = 0; idx < actions.size(); idx++) {
		msg += actions.get(idx)->getHTMLController(
				"/set?id=" + String(idx) + "&");
		msg += "<br>";
	}
	return msg;
}

void Controller::setRequest(String id, String key, String value) {
	Serial.println("setRequest");
	int idx = id.toInt();
	Serial.println(idx);
	if (idx >= actions.size()) {
		return;
	}
	Serial.println("Actions");
	actions.get(idx)->setSettings(key, value);
}

void Controller::notifyDCCSpeed(int id, int speed, int direction,
		int SpeedSteps) {
	// Filter out known commands
	LocData* data;
	if (items.find(id) == items.end()) {
		data = new LocData();
		items[id] = data;
	} else {
		data = items[id];
		if (data->direction == direction && data->speed == speed
				&& data->speedsteps == SpeedSteps) {
			return;
		}
	}

	// Save new state
	data->direction = direction;
	data->speed = speed;
	data->speedsteps = SpeedSteps;
	Serial.println(
			"DCC-Speed: " + String(id) + " D: " + String(direction) + " "
					+ String(speed) + " " + String(SpeedSteps));

	// Send the information to the actions
	int idx;
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->DCCSpeed(id, speed, direction, SpeedSteps);
	}
}


void Controller::notifyDCCFun(int id, int startbit, int stopbit, unsigned long partValues) {
	// Get the old status ...
	FuncData* data;
	if (funcdatas.find(id) == funcdatas.end()) {
		data = new FuncData();
		funcdatas[id] = data;
	} else {
		data = funcdatas[id];
	}
	boolean changed = false;
	// .. and send only the changed bits
	unsigned long int value = data->status;
	for (int i = startbit; i <= stopbit; i++) {
		unsigned long int oldBitValue = bit_is_set(value, i);
		unsigned long int newBitValue = bit_is_set(partValues, i);
		if (oldBitValue == newBitValue) {
			continue;
		}
		changed = true;
		if (newBitValue == 0) {
			clear_bit(data->status, i);
		} else {
			set_bit(data->status, i);
		}
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, i, (newBitValue == 0) ? 0 : 1);
		}
	}
	if (changed) {
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, data->status);
		}
	}
}

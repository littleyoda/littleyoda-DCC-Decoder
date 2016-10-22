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
	loops.add(base);
}

void Controller::doLoops() {
	int idx;
	while (nextRun.size() < loops.size()) {
		nextRun.add(0);
	}
	// Run and save the new execute time
	unsigned long now = millis();
	for (idx = 0; idx < loops.size(); idx++) {
		if (nextRun.get(idx)  <= now) {
			int wait = loops.get(idx)->loop();
			nextRun.set(idx, now + wait);
		}
	}
}

void Controller::registerAction(ActionBase* base) {
	actions.add(base);
	loops.add(base);
}

void Controller::registerLoop(interfaceLoop* loop) {
	loops.add(loop);
}

void Controller::notifyTurnout(int id, int direction, int source) {
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
		actions.get(idx)->TurnoutCmd(id, direction, source);
	}
}

String Controller::getHTMLController() {
	String msg = "<div class=\"container\">";
	for (int idx = 0; idx < actions.size(); idx++) {
		msg += actions.get(idx)->getHTMLController(
				"/set?id=" + String(idx) + "&");
		msg += "\n";
	}
	msg += "</div>";
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
		int SpeedSteps, int source) {
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
		actions.get(idx)->DCCSpeed(id, speed, direction, SpeedSteps, source);
	}
}


void Controller::notifyDCCFun(int id, int startbit, int stopbit, unsigned long partValues, int source) {
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
			actions.get(idx)->DCCFunc(id, i, (newBitValue == 0) ? 0 : 1, source);
		}
	}
	if (changed) {
		Serial.println("Func " + String(id) + " " + String(data->status));
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, data->status, source);
		}
	}
}

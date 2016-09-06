/*
 * ActionTurnOut.cpp
 *
 *  Created on: 19.08.2016
 *      Author: sven
 */

#include "Arduino.h"
#include "ActionTurnOut.h"
#include "Logger.h"

ActionTurnOut::ActionTurnOut(int dir1, int dir2, int enable, int id) {
	Logger::getInstance()->addToLog(
			"Turnout Dir " + String(dir1) + "/" + String(dir2) + " Enable: "
					+ enable + " ID: " + id);
	dirPin[0] = dir1;
	dirPin[1] = dir2;
	this->enable = enable;
	this->id = id;
	int i;
	for (i = 0; i < 2; i++) {
		pinMode(dirPin[i], OUTPUT);
		digitalWrite(dirPin[i], 0);
	}
	pinMode(enable, OUTPUT);
	off();

}

String ActionTurnOut::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionTurnOut::getHTMLController(String urlprefix) {
	String message = "Turnout-ID ";
	message += String(id);
	message += " <a href=\"";
	message += urlprefix;
	message += "value=0";
	message += "\">Flip to 0</a>";
	message += " <a href=\"";
	message += urlprefix;
	message += "value=1";
	message += "\">Flip to 1</a>";
	return message;
}

void ActionTurnOut::off() {
	digitalWrite(enable, 0);
	for (int i = 0; i < 2; i++) {
		digitalWrite(dirPin[i], 0);
	}
}

void ActionTurnOut::loop() {
	off();
}

void ActionTurnOut::TurnoutCmd(int id, int status) {
	if (id != this->id) {
		return;
	}
	if (status == 1) {
		digitalWrite(dirPin[0], 0);
		digitalWrite(dirPin[1], 1);
	} else {
		digitalWrite(dirPin[0], 1);
		digitalWrite(dirPin[1], 0);
	}
	digitalWrite(enable, 1);
	delay(200);
	off();
}

ActionTurnOut::~ActionTurnOut() {
}

void ActionTurnOut::setSettings(String key, String value) {
	TurnoutCmd(id, value.toInt());
}


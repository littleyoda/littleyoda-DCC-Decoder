/*
 c++	* ActionLed.cpp
 *
 *  Created on: 27.08.2016
 *      Author: sven
 */

#include "ActionLed.h"
#include "Logger.h"

ActionLed::ActionLed(int gpio, int locoId, int func) {
	Logger::getInstance()->addToLog("Starting LED  " + String(gpio) + " " + String(locoId) + " " + String(func));
	pinMode(gpio, OUTPUT);
	this->gpio = gpio;
	this->locoId = locoId;
	this->func = func;
}

ActionLed::~ActionLed() {
}

String ActionLed::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionLed::getHTMLController(String urlprefix) {
	String message = "LED ";
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

void ActionLed::setSettings(String key, String value) {
	int status = value.toInt();
	setSettings(status);
}

void ActionLed::setSettings(int status) {
	Logger::getInstance()->addToLog("Led " + String(gpio) + " changed to " + status);
	if (status == 0) {
		digitalWrite(gpio, 0);
	} else if (status == 1) {
		digitalWrite(gpio, 1);
	}
}

void ActionLed::DCCFunc(int id, int bit, int newvalue) {
	if (id == this->locoId && bit == this->func) {
		setSettings(newvalue);
	}
}

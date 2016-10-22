/*
 * ActionServo.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include "ActionServo.h"
#include "Logger.h"

ActionServo::ActionServo(int pin, int id) {
	Logger::getInstance()->addToLog(
			"Starting Servo GPIO: " + String(pin) + " Loco: " + String(id));
	this->pin = pin;
	this->id = id;
}

ActionServo::~ActionServo() {
}

int ActionServo::loop() {
	if (lastcommand == 0) {
		return 2000;
	}
	// Autodetach after 2 sec
	if (millis() > lastcommand + 2000) {
		lastcommand = 0;
		detach();
		return 2000;
	}
	return 20;
}

String ActionServo::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionServo::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "Servo ";
	message += "</div><div class=\"column column-90\">";

	message += " <a class=\"button\" href=\"";
	message += urlprefix;
	message += "value=";
	message += String(-1);
	message += "\">";
	message += "disable";
	message += "</a>";

	for (int i = 0; i <= 180; i = i + 10) {
		message += " <a class=\"button\" href=\"";
		message += urlprefix;
		message += "value=";
		message += String(i);
		message += "\">";
		message += String(i);
		message += "°</a>";
	}
	message += "</div>";
	message += "</div>";
	return message;
}

void ActionServo::setSettings(String key, String value) {
	setSettings(value.toInt());
}

void ActionServo::setSettings(int status) {
	Logger::getInstance()->addToLog(
			"Servo " + String(pin) + " changed to " + status);
	if (status == -1) {
		detach();
	} else {
		myservo.write(status);
		attach();
		lastcommand = millis();
	}
}

void ActionServo::TurnoutCmd(int id, int status) {
}

void ActionServo::attach() {
	if (isAttach) {
		return;
	}
	myservo.attach(pin);
	isAttach = true;
}

void ActionServo::detach() {
	if (!isAttach) {
		return;
	}
	myservo.detach();
	isAttach = false;
}

void ActionServo::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	if (id != this->id) {
		return;
	}
	if (direction == -1) {
		setSettings(-1);
		return;
	}
	int r = speed * 180 / SpeedSteps;
	setSettings(r);
}

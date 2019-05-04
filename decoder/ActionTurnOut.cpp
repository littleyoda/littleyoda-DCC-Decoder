/*l
 * ActionTurnOut.cpp
 *
 *  Created on: 19.08.2016
 *      Author: sven
 */

#include "Arduino.h"
#include "ActionTurnOut.h"
#include "Logger.h"
#include "GPIO.h"

ActionTurnOut::ActionTurnOut(int dir1, int dir2, int enable) {
	Logger::log("Starting Turnout Dir " + String(dir1) + "/" + String(dir2) + " Enable: " + enable);
	dirPin[0] = dir1;
	dirPin[1] = dir2;
	this->enable = enable;
	int i;
	for (i = 0; i < 2; i++) {
		GPIO.pinMode(dirPin[i], OUTPUT, "Weiche Richtung " + String(0));
		GPIO.digitalWrite(dirPin[i], 0);
	}
	GPIO.pinMode(enable, OUTPUT, "Weiche Enable");
	off();

}

String ActionTurnOut::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionTurnOut::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "Turnout-ID ";
	message += getName();
	message += "</div>";
	message += "<div class=\"column column-90\"><a style=\"font-size: 4rem;\" class=\"button\" href=\"";
	message += urlprefix;
	message += "value=0";
	message += "\">&#9756;</a>";
	message += " <a style=\"font-size: 4rem;\" class=\"button\" href=\"";
	message += urlprefix;
	message += "value=1";
	message += "\">&#9758;</a></div>";
	message += "</div>";
	return message;
}

void ActionTurnOut::off() {
	GPIO.digitalWrite(enable, 0);
	for (int i = 0; i < 2; i++) {
		GPIO.digitalWrite(dirPin[i], 0);
	}
}

int ActionTurnOut::loop() {
	off();
	return 1000;
}

ActionTurnOut::~ActionTurnOut() {
}

void ActionTurnOut::setSettings(String key, String value) {
	int status = value.toInt();
	if (status == 1) {
		GPIO.digitalWrite(dirPin[0], 0);
		GPIO.digitalWrite(dirPin[1], 1);
	} else {
		GPIO.digitalWrite(dirPin[0], 1);
		GPIO.digitalWrite(dirPin[1], 0);
	}
	GPIO.digitalWrite(enable, 1);
	delay(200);
	off();
}


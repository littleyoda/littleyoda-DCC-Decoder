/*
 c++	* ActionLed.cpp
 *
 *  Created on: 27.08.2016
 *      Author: sven
 */

#include <Arduino.h>
#include <stdlib.h>
#include "ActionLed.h"
#include "Logger.h"
#include "Utils.h"

ActionLed::ActionLed(Pin* g) {
	gpio = g;
	Logger::getInstance()->addToLog("Starting LED  " + g->toString());
	GPIO.pinMode(gpio, OUTPUT, "LED");
}

ActionLed::~ActionLed() {
}

String ActionLed::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionLed::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\">";
	message += " <div class=\"column column-10\">LED</div>";
	message += "<div class=\"column column-90\"><a class=\"button button-black\" href=\"";
	message += urlprefix;
	message += "value=0";
	message += "\">&#x1f4a1;Aus</a>";
	message += " <a class=\"button button-white\" href=\"";
	message += urlprefix;
	message += "value=1";
	message += "\">An</a></div>";
	message += "</div>";
	return message;

}

void ActionLed::setSettings(String key, String value) {
	int status = value.toInt();
	setSettings(status);
}

void ActionLed::setSettings(int status) {
	Logger::getInstance()->addToLog("Led " + String(gpio) + " changed to " + String(status));
	if (status == 0) {
		GPIO.digitalWrite(gpio, 0);
		currentStatus = 0;
	} else if (status == 1) {
		GPIO.digitalWrite(gpio, 1);
		currentStatus = 1;
	}
}


int ActionLed::loop() {
	return -1; // TODO Pattern
}

void ActionLed::setPattern(const char* patternString) {
	for (int i = 0; patternString[i] != 0; i++) {
		Logger::getInstance()->addToLog("GPIO " + String(patternString[i]) + " " + String(Utils::hextoint(patternString[i])));
	}
}

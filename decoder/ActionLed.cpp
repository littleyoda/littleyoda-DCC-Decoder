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
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting LED  " + g->toString());
	GPIOobj.pinMode(gpio, OUTPUT, "LED");
}

ActionLed::~ActionLed() {
}


String ActionLed::getHTMLController(String urlprefix) {
	String message =  F("<div class=\"row\">");
	message += F(" <div class=\"column column-10\">LED</div>");
	message += F("<div class=\"column column-90\"><a class=\"button button-black\" href=\"");
	message += urlprefix;
	message += F("value=0");
	message += F("\">&#x1f4a1;Aus</a>");
	message += F(" <a class=\"button button-white\" href=\"");
	message += urlprefix;
	message += F("value=1");
	message += F("\">An</a></div>");
	message += "</div>";
	return message;

}

void ActionLed::setSettings(String key, String value) {
	int status = value.toInt();
	setSettings(status);
}

void ActionLed::setSettings(int status) {
	if (currentStatus == status) {
		return;
	}
	Logger::getInstance()->addToLog(LogLevel::TRACE, "Led " + gpio->toString() + " changed to " + String(status));
	if (status == 0) {
		GPIOobj.digitalWrite(gpio, 0);
		currentStatus = 0;
	} else if (status == 1) {
		GPIOobj.digitalWrite(gpio, 1);
		currentStatus = 1;
	}
}


//int ActionLed::loop() {
//	return 3000 + ; // TODO Pattern
//}

void ActionLed::setPattern(const char* patternString) {
//	for (int i = 0; patternString[i] != 0; i++) {
//		Logger::getInstance()->addToLog("GPIO " + String(patternString[i]) + " " + String(Utils::hextoint(patternString[i])));
//	}
}

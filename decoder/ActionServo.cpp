/*
 * ActionServo.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include "ActionServo.h"
#include "Logger.h"
#include "GPIO.h"

//ifdef ESP8266

ActionServo::ActionServo(int pin) {
	Logger::getInstance()->addToLog(LogLevel::INFO, 
			"Starting Servo GPIO: " + String(pin));
	GPIOobj.pinMode(pin, OUTPUT, "Servo");
	this->pin = pin;
	setModulName("Servo");
	setConfigDescription("pin: " + String(pin));

}

ActionServo::~ActionServo() {
}

int ActionServo::loop() {
	
	return 20;
}

String ActionServo::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionServo::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "Servo ";
	message += "</div><div class=\"column column-90\">";

	message += "<a href=\"#\" onclick=\"";
	message += "send('" + urlprefix + "&key=so&value=-1')";
	message += "\">Off";
	message += "°</a> \r\n";

	for (int i = 0; i <= 180; i = i + 10) {
		String action = "send('" + urlprefix + "&key=so&value=" + i + "')";
		message += "<a href=\"#\" onclick=\"";
		message += action;
		message += "\">";
		message += String(i);
		message += "°</a> \r\n";
	}
	message += "</div>";
	message += "</div>";
	return message;
}

void ActionServo::setSettings(String key, String value) {
	Logger::getInstance()->addToLog(LogLevel::TRACE,
			"Servo Value: " + value);
	int status = value.toInt();
	if (status == -1) {
		return;
	} else {
		GPIOobj.servoWrite(pin, status);	
  } 
}

//endif

/*
 * ActionTesting.cpp
 *
 *  Created on: 04.09.2016
 *      Author: sven
 */

#include "Arduino.h"
#include "ActionTesting.h"

ActionTesting::ActionTesting(int gpio) {
	this->gpio = gpio;
	status = 0;
	pinMode(gpio, INPUT);
	pinMode(D1, OUTPUT);
	// TODO Auto-generated constructor stub

}

ActionTesting::~ActionTesting() {
	// TODO Auto-generated destructor stub
}

String ActionTesting::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionTesting::getHTMLController(String urlprefix) {
	return "";
}

void ActionTesting::setSettings(String key, String value) {
}

void ActionTesting::loop() {
//	//int currentstatus = digitalRead(gpio);
//	if (millis() > last + 1000) {
//		unsigned int x = (unsigned int) analogRead(A0);
//		Serial.println("Wert: " + String(x));
//		last = millis();
//	}
	int currentstatus = digitalRead(gpio);
	if (currentstatus != status) {
		status = currentstatus;
		digitalWrite(D1, status);
		Serial.println(String(millis()) + "/" + String(millis() - last) + ": Now " + String(status));
		last = millis();
	}
}

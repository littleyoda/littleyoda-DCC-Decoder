/*
 * ActionStepperOutput.cpp
 *
 *  Created on: 23.01.2019
 *      Author: sven
 */



#include "ActionStepperOutput.h"
ActionStepperOutput::ActionStepperOutput(Pin* p1, Pin* p2, Pin* p3, Pin* p4) {
	pins[0] = p1;
	pins[1] = p2;
	pins[2] = p3;
	pins[3] = p4;
	for (int i = 0; i < 4; i++) {
		GPIO.pinMode(pins[i], OUTPUT, "Stepper");
	}
}

ActionStepperOutput::~ActionStepperOutput() {
}


int ActionStepperOutput::loop() {
	if (current == target) {
		for (int i = 0; i < 4; i++) {
			GPIO.digitalWrite(pins[i], 0);
		}
		return 50;
	}
	if (current < target) {
		state++;
		current++;
		if (state == 8) {
			state = 0;
		}
	} else {
		state--;
		current--;
		if (state == -1) {
			state = 7;
		}
	}
	for (int i = 0; i < 4; i++) {
		GPIO.digitalWrite(pins[i], steps[state][i]);
	}
	return 1;
}

void ActionStepperOutput::setSettings(String key, String value) {
	if (value.toInt() != 0) {
		target = value.toInt();
	}
}

String ActionStepperOutput::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "Stepper";
	message += "</div><div class=\"column column-90\">";
	message += "Ist-Position " + String(current) + " (Zum aktualisieren, Seite neu laden)<br>";
	message += "Soll-Position " + String(target) + " (Zum aktualisieren, Seite neu laden) <br>";

	int steps = range / 100;
	for (int i = 0; i <= range; i = i +steps) {
		String action = "send('" + urlprefix + "&key=so&value=" + i + "')";
		message += "<a href=\"#\" onclick=\"";
		message += action;
		message += "\">";
		message += String(i);
		message += "</a> \r\n";
	}
	message += "</div>";
	message += "</div>";
	return message;
}

/*
 * ActionStepperOutput.cpp
 *
 *  Created on: 23.01.2019
 *      Author: sven
 */



#include "ActionStepperOutput.h"
#include <FS.h>
#ifdef ESP32
	#include "SPIFFS.h"
#endif


ActionStepperOutput::ActionStepperOutput(Pin* p1, Pin* p2, Pin* p3, Pin* p4, boolean _persistent) {
	Logger::log(LogLevel::INFO, "Stepper started!");
	pins[0] = p1;
	pins[1] = p2;
	pins[2] = p3;
	pins[3] = p4;
	for (int i = 0; i < 4; i++) {
		GPIOobj.pinMode(pins[i], OUTPUT, "Stepper");
	}
	persistent = _persistent;
	setModulName("Stepper");
	setConfigDescription("Pin: " + p1->toString() + " " + p2->toString() + " " + p3->toString() + " " + p4->toString());

}

ActionStepperOutput::~ActionStepperOutput() {
}

void ActionStepperOutput::load() {
	if (persistent && SPIFFS.exists("/" + getName() + ".dat")) {
		File dataFile = SPIFFS.open("/" + getName() + ".dat" , "r");
		current = dataFile.parseInt();
		lastsaved = current;
		target = current;
		Logger::log(LogLevel::INFO, "Gespeicherte Position: " + String(current));
	}
}

int ActionStepperOutput::loop() {
	if (current == target) {
		for (int i = 0; i < 4; i++) {
			GPIOobj.digitalWrite(pins[i], 0);
		}
		save();
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
		GPIOobj.digitalWrite(pins[i], steps[state][i]);
	}
	return 1;
}

void ActionStepperOutput::setSettings(String key, String value) {
	if (key.equals("ap")) {
		target = value.toInt();
		current = target;
		Logger::getInstance()->printf(LogLevel::INFO, "Setting Position to %d", current);
	} else {
		if (value.toInt() != 0) {
			target = value.toInt();
		}
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

String ActionStepperOutput::getHTMLCfg(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += getName();
	message += "</div>";
	message += "<div class=\"column column-90\">";
	message += "<a href=\"#\" onclick=\"";
	message += "send('" + urlprefix + "&key=ap&value=0')";;
	message += "\">";
	message += "Aktuelle Position als Nullpunkt festlegen";
	message += "</a> \r\n";
	message += "</div>";
	return message;
}

void ActionStepperOutput::save() {
	if (persistent && current != lastsaved) {
		File dataFile = SPIFFS.open("/" + getName() + ".dat" , "w");
		dataFile.print(String(current));
		dataFile.close();
		lastsaved = current;
	}
}
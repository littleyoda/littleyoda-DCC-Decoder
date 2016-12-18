/*
 * ActionPWMOutput.cpp
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "ActionPWMOutput.h"
#include "Consts.h"
#include "Utils.h"

ActionPWMOutput::ActionPWMOutput(int locoId, uint8_t pwm, uint8_t forward, uint8_t reverse) {
	Logger::getInstance()->addToLog("Starting PWM...");
	Logger::getInstance()->addToLog("PWM-Pin: "
			+ Utils::gpio2string(pwm) + " Forward-Pin: "
			+ Utils::gpio2string(forward) + " Reverse-Pin: "
			+ Utils::gpio2string(reverse)
			+ " Addr: " + String(locoId)
	);
	  analogWriteFreq(100);
	  locid = locoId;
	  gpioPWM = pwm;
	  gpioForward = forward;
	  gpioReverse = reverse;
	  pinMode(gpioPWM, OUTPUT); digitalWrite(gpioPWM, LOW); // PWM Signal
	  pinMode(gpioForward, OUTPUT); digitalWrite(gpioForward, LOW); // Forward
	  pinMode(gpioReverse, OUTPUT); digitalWrite(gpioReverse, LOW); // Reverse
	  setDirection(1);
}

ActionPWMOutput::~ActionPWMOutput() {
}

int ActionPWMOutput::loop() {
	return 10000;
}

String ActionPWMOutput::getHTMLCfg(String urlprefix) {
	String message = "";
	message += "<div class=\"column column-90\">Freq: ";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(100) + "\">" + String(100) + "</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(250) + "\">" + String(250) + "</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(500) + "\">" + String(500) + "</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(1000) + "\">" + String(1000) + "</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(5000) + "\">" + String(5000) + "</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=freq&value=" +  String(10000) + "\">" + String(10000) + "</a>\n";
	message += "</div>";
	return message;
}

String ActionPWMOutput::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "PWM ";
	message += "</div>";
	message += "<div class=\"column column-90\">Speed: ";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=dir&value=" +  String(-1) + "\">&#x1F850;</a>\n";
	message += " <a class=\"button\" href=\"" + urlprefix + "key=dir&value=" +  String(1) + "\">&#x1F852;</a>\n";

	for (int i = 0; i <= 100; i = i + 10) {
		message += " <a class=\"button\" href=\"";
		message += urlprefix;
		message += "key=speed&value=";
		message += String(i);
		message += "\">";
		message += String(i);
		message += "%</a>";
	}
	message += "</div>";

	message += "</div>";
	return message;
}

void ActionPWMOutput::setSettings(String key, String value) {
	Serial.println(key + " " + value);
	if (key.equals("speed")) {
		Serial.println("Speed");
		int v = PWMRANGE * value.toInt() / 100;
		setSpeedInProcent(v);
	} else if (key.equals("freq")) {
		Serial.println("Freq");
		analogWriteFreq(value.toInt());
	} else if (key.equals("dir")) {
		Serial.println("Direction");
		setDirection(value.toInt());
	}
}

void ActionPWMOutput::setDirection(int dir) {
	if (dir == 1) {
		digitalWrite(gpioForward, HIGH);
		digitalWrite(gpioReverse, LOW);
		direction = 1;
	} else if (dir == -1) {
		digitalWrite(gpioForward, LOW);
		digitalWrite(gpioReverse, HIGH);
		direction = -1;
	} else {
		digitalWrite(gpioForward, LOW);
		digitalWrite(gpioReverse, LOW);
		direction = 0;
	}
}

void ActionPWMOutput::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	if (id == locid || id == Consts::LOCID_ALL) {
		if (speed == Consts::SPEED_EMERGENCY || speed == Consts::SPEED_STOP) {
			speed = 0;
		}
		setDirection(direction);
		int v = PWMRANGE * speed / SpeedSteps;
		setSpeedInProcent(v);
	}
}

void ActionPWMOutput::setSpeedInProcent(int speedProc) {
	analogWrite(gpioPWM, speedProc);
}

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
#include "GPIO.h"

ActionPWMOutput::ActionPWMOutput(uint8_t pwm, uint8_t forward, uint8_t reverse) {
	Logger::getInstance()->addToLog("Starting PWM...");
	Logger::getInstance()->addToLog("PWM-Pin: "
			+ GPIOobj.gpio2string(pwm) + " Forward-Pin: "
			+ GPIOobj.gpio2string(forward) + " Reverse-Pin: "
			+ GPIOobj.gpio2string(reverse)
	);
	  GPIOobj.analogWriteFreq(100);
	  gpioPWM = pwm;
	  gpioForward = forward;
	  gpioReverse = reverse;
	  String fctname = " PWM Signal";
	  if (gpioPWM != Consts::DISABLE) {
		  GPIOobj.pinMode(gpioPWM, OUTPUT, "PWM: PWM Signal"); GPIOobj.digitalWrite(gpioPWM, LOW); // PWM Signal
		  fctname = "";
	  }
	  GPIOobj.pinMode(gpioForward, OUTPUT, "PWM: Forward" + fctname); GPIOobj.digitalWrite(gpioForward, LOW); // Forward
	  GPIOobj.pinMode(gpioReverse, OUTPUT, "PWM: Reverse" + fctname); GPIOobj.digitalWrite(gpioReverse, LOW); // Reverse
	  setDirection(1);
}

ActionPWMOutput::~ActionPWMOutput() {
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
	String action = "document.getElementById('pwmValue').textContent=this.value; send('" + urlprefix + "&key=sd&value='+this.value)";
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "PWM ";
	message += "</div>";
	message += "<div class=\"column column-90\">";
	message +=  "<div id=\"pwmValue\">0</div>";
	message += " <input type=range min=-100 max=100 value=0 step=10 oninput=\"" + action + "\" onchange=\"" + action + "\"><br/>";
	message += "</div>";

	message += "</div>";
	return message;
}

void ActionPWMOutput::setSettings(String key, String value) {
	Serial.println(key + " " + value);
	if (key.equals("sd")) {
		int v = value.toInt();
		if (v < 0) {
			setDirection(-1);
		} else {
			setDirection(1);
		}
		Serial.println("Speed");
		int s = PWMRANGE * abs(value.toInt()) / 100;
		setSpeedInProcent(s);
	} else if (key.equals("freq")) {
		Serial.println("Freq");
		GPIOobj.analogWriteFreq(value.toInt());
	}
}

void ActionPWMOutput::setDirection(int dir) {
	if (gpioPWM == Consts::DISABLE) {
		handleSpeedandDirectionWithoutPWMPin(dir, currentSpeed);
		return;
	}
	if (dir == 1) {
		GPIOobj.digitalWrite(gpioForward, HIGH);
		GPIOobj.digitalWrite(gpioReverse, LOW);
		direction = 1;
	} else if (dir == -1) {
		GPIOobj.digitalWrite(gpioForward, LOW);
		GPIOobj.digitalWrite(gpioReverse, HIGH);
		direction = -1;
	} else {
		Serial.println("Error: Direction " + String(dir));
		GPIOobj.digitalWrite(gpioForward, LOW);
		GPIOobj.digitalWrite(gpioReverse, LOW);
		direction = 0;
	}
}

//void ActionPWMOutput::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
//	if (id == locid || id == Consts::LOCID_ALL) {
//		if (speed == Consts::SPEED_EMERGENCY || speed == Consts::SPEED_STOP) {
//			speed = 0;
//		}
//		setDirection(direction);
//		int v = PWMRANGE * speed / SpeedSteps;
//		setSpeedInProcent(v);
//	}
//}

void ActionPWMOutput::setSpeedInProcent(int speedProc) {
	if (gpioPWM == Consts::DISABLE) {
		handleSpeedandDirectionWithoutPWMPin(direction, speedProc);
		return;
	}
	GPIOobj.analogWrite(gpioPWM, speedProc);
}

void ActionPWMOutput::handleSpeedandDirectionWithoutPWMPin(int dir, int speed) {
	currentSpeed = speed;
	if (dir == 1) {
		GPIOobj.analogWrite(gpioForward, currentSpeed);
		GPIOobj.analogWrite(gpioReverse, 0);
		direction = 1;
	} else if (dir == -1) {
		GPIOobj.analogWrite(gpioForward, 0);
		GPIOobj.analogWrite(gpioReverse, currentSpeed);
		direction = -1;
	} else {
		Serial.println("Error: Direction " + String(dir));
		GPIOobj.digitalWrite(gpioForward, LOW);
		GPIOobj.digitalWrite(gpioReverse, LOW);
		direction = 0;
	}
}

/*
 * ActionPWMDirect.cpp
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "ActionPWMDirect.h"
#include "Consts.h"
#include "Utils.h"
#include "GPIO.h"

ActionPWMDirect::ActionPWMDirect(int pwm, int forward, int reverse) {
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting PWM...");
	Logger::getInstance()->addToLog(LogLevel::INFO, "PWM-Pin: "
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
	setModulName("PWM Out");
	setConfigDescription("");
}

ActionPWMDirect::~ActionPWMDirect() {
}

void ActionPWMDirect::setFreq(uint32_t value) {
	Logger::log(LogLevel::TRACE, "Frequenz: " + String(value));
	GPIOobj.analogWriteFreq(value);
}


void ActionPWMDirect::setSettings(String key, String value) {
	Logger::log(LogLevel::TRACE, "PWM: SetSettings " + key + "/" + value);
	if (key.equals("sd")) {
		int v = value.toInt();
		if (v < 0) {
			setDirection(-1);
		} else {
			setDirection(1);
		}

		int s = GPIOobj.getPWMRange() * handlePwmRampe((value.toInt())) / 127;
		setSpeedInProcent(s);
	} else if (key.equals("freq")) {
		setFreq(value.toInt());
	}
}

void ActionPWMDirect::setDirection(int dir) {
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
		Logger::log(LogLevel::ERROR, "Error: Direction " + String(dir));
		GPIOobj.digitalWrite(gpioForward, LOW);
		GPIOobj.digitalWrite(gpioReverse, LOW);
		direction = 0;
	}
}


void ActionPWMDirect::setSpeedInProcent(int speedProc) {
	if (gpioPWM == Consts::DISABLE) {
		handleSpeedandDirectionWithoutPWMPin(direction, speedProc);
		return;
	}
	GPIOobj.analogWrite(gpioPWM, speedProc);
}

void ActionPWMDirect::handleSpeedandDirectionWithoutPWMPin(int dir, int speed) {
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
		Logger::log(LogLevel::ERROR, "Error: Direction " + String(dir));
		GPIOobj.digitalWrite(gpioForward, LOW);
		GPIOobj.digitalWrite(gpioReverse, LOW);
		direction = 0;
	}
}

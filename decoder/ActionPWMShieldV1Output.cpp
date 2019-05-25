/*
 * ActionPWMSchieldV1Output.cpp
 *
 * 
 *  TODO Frequenz
 * 
 *  Created on: 09.12.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "ActionPWMShieldV1Output.h"
#include "Consts.h"
#include "Utils.h"
#include "Logger.h"


ActionPWMSchieldV1Output::ActionPWMSchieldV1Output(uint8_t _i2caddr, uint8_t _motoraddr) {
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting PWM(ShieldV1) ... I2C: " + String(_i2caddr) + "/" + String(_motoraddr));
	motoraddr = _motoraddr;
	i2caddr = _i2caddr;
	if (motoraddr < 0 || motoraddr > 1) {
		Logger::getInstance()->printf(LogLevel::ERROR, "Motoraddresse ungültig: %d", _motoraddr);
	}
}

ActionPWMSchieldV1Output::~ActionPWMSchieldV1Output() {
}


void ActionPWMSchieldV1Output::setSettings(String key, String value) {
	Logger::log(LogLevel::TRACE, "SetSettings " + key + "/" + value);
	if (key.equals("sd")) {
		setSpeed(value.toInt());
		// int s = PWMRANGE * abs(value.toInt()) / 100;
		// setSpeedInProcent(s);
	} else if (key.equals("freq")) {
		Logger::log(LogLevel::ERROR, "Freq nicht implementiert");
	}
}





void ActionPWMSchieldV1Output::setSpeed(long l) {
	uint16_t _pwm_val;

	Wire.beginTransmission(i2caddr);
	Wire.write(motoraddr | (byte)0x10);
	// #define _SHORT_BRAKE 0
	// #define _CCW  1
	// #define _CW 	2
	// #define _STOP 3
	// #define _STANDBY 4
	uint8_t dir = 3; // or 0?
	if (l > 0) {
		dir = 1;
	} 
	if (l < 0) {
		dir = 2;
	}
	Wire.write(dir);

	_pwm_val=uint16_t(abs(l) * 100.0f);

	if(_pwm_val>10000)
		_pwm_val=10000;

	Wire.write((byte)(_pwm_val >> 8));
	Wire.write((byte)_pwm_val);
	Wire.endTransmission();     // stop transmitting
}

void ActionPWMSchieldV1Output::setfreq(uint32_t freq) {
	Wire.beginTransmission(i2caddr);
	Wire.write(((byte)(freq >> 16)) & (byte)0x0f);
	Wire.write((byte)(freq >> 16));
	Wire.write((byte)(freq >> 8));
	Wire.write((byte)freq);
	Wire.endTransmission();     // stop transmitting
}

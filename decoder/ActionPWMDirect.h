/*
 * ActionPWMDirect.h
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#ifndef ActionPWMDirect_H_
#define ActionPWMDirect_H_

#include <Arduino.h>

#include "ActionPWMOutput.h"

#ifndef ESP8266

// HACK
#define PWMRANGE 1024

#endif

class ActionPWMDirect: public ActionPWMOutput {
public:
	ActionPWMDirect(int  pwm, int forward, int reverse);
	virtual ~ActionPWMDirect();

private:
	virtual void setSettings(String key, String value);
	void setDirection(int dir);
	void handleSpeedandDirectionWithoutPWMPin(int dir, int currentSpeed);
	void setSpeedInProcent(int proc);
	int direction = 1;
	int currentSpeed = 0;
	int gpioPWM;
	int gpioForward;
	int gpioReverse;
	uint8_t* arr;
};

#endif /* ActionPWMDirect_H_ */

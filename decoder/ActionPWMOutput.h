/*
 * ActionPWMOutput.h
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#ifndef ACTIONPWMOUTPUT_H_
#define ACTIONPWMOUTPUT_H_

#include <Arduino.h>

#include "ISettings.h"

#ifndef ESP8266

// HACK
#define PWMRANGE 1024

#endif

class ActionPWMOutput: public ISettings {
public:
	ActionPWMOutput(uint8_t  pwm, uint8_t forward, uint8_t reverse);
	virtual ~ActionPWMOutput();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
private:
	virtual void setSettings(String key, String value);
	virtual void setDirection(int dir);
	void handleSpeedandDirectionWithoutPWMPin(int dir, int currentSpeed);
	virtual void setSpeedInProcent(int proc);
	int direction = 1;
	int currentSpeed = 0;
	int gpioPWM;
	int gpioForward;
	int gpioReverse;
};

#endif /* ACTIONPWMOUTPUT_H_ */

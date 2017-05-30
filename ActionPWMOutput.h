/*
 * ActionPWMOutput.h
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#ifndef ACTIONPWMOUTPUT_H_
#define ACTIONPWMOUTPUT_H_

#include <Arduino.h>
#include "ActionBase.h"

class ActionPWMOutput: public ActionBase {
public:
	ActionPWMOutput(int locoId, uint8_t  pwm, uint8_t forward, uint8_t reverse);
	virtual ~ActionPWMOutput();
	virtual int loop();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
	virtual void setDirection(int dir);
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
private:
	void handleSpeedandDirection(int dir, int currentSpeed);
	virtual void setSpeedInProcent(int proc);
	int direction = 1;
	int currentSpeed = 0;
	int locid;
	int gpioPWM;
	int gpioForward;
	int gpioReverse;
};

#endif /* ACTIONPWMOUTPUT_H_ */

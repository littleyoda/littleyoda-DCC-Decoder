/*
 * ActionPWMOutput.h
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#ifndef ACTIONPWMPID_H_
#define ACTIONPWMPID_H_

#include <Arduino.h>

#include "ActionPWMOutput.h"


class ActionPWMPid: public ActionPWMOutput {
public:
	ActionPWMPid();
	virtual ~ActionPWMPid();
	void setSpeed(long l);

private:
	virtual void setSettings(String key, String value);

//	void setDirection(int dir);
///	void setSpeedInProcent(int proc);
	int direction = 1;
	int currentSpeed = 0;

};

#endif /* ACTIONPWMOUTPUT_H_ */

/*
 * ActionPWMOutput.h
 *
 *  Created on: 09.12.2016
 *      Author: sven
 */

#ifndef ACTIONPWMV1OUTPUT_H_
#define ACTIONPWMV1OUTPUT_H_

#include <Arduino.h>

#include "ActionPWMOutput.h"

#ifndef ESP8266

// HACK
#define PWMRANGE 1024

#endif

class ActionPWMSchieldV1Output: public ActionPWMOutput {
public:
	ActionPWMSchieldV1Output(uint8_t _i2caddr, uint8_t _motoraddr);
	virtual ~ActionPWMSchieldV1Output();

private:
	virtual void setSettings(String key, String value);
	void setSpeed(long l);

//	void setDirection(int dir);
///	void setSpeedInProcent(int proc);
	int direction = 1;
	int currentSpeed = 0;
	uint8_t motoraddr;
	uint8_t i2caddr;
	uint8_t* arr;
	void setfreq(uint32_t freq);
};

#endif /* ACTIONPWMOUTPUT_H_ */

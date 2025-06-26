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

class ActionPWMOutput: public ISettings {
public:
	ActionPWMOutput();
	virtual ~ActionPWMOutput();
	virtual String getHTMLController(String urlprefix);
	virtual void getHTMLConfig(String urlprefix, Controller* c);
	void setPwmProfil(uint8_t* arr);
	long handlePwmProfil(long pos);
	virtual void setFreq(uint32_t value);

private:
	uint8_t* pwmRampe = nullptr;
};

#endif /* ACTIONPWMOUTPUT_H_ */

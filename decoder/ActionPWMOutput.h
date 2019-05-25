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
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	void setPwmValues(uint8_t* arr);
	uint8_t getValue(uint8_t pos);

private:
	uint8_t* arr;
};

#endif /* ACTIONPWMOUTPUT_H_ */

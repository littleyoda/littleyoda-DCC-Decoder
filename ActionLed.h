/*
 * ActionLed.h
 *
 *  Created on: 27.08.2016
 *      Author: sven
 */

#ifndef ACTIONLED_H_
#define ACTIONLED_H_

#include <Arduino.h>
#include "ActionBase.h"

class ActionLed: public ActionBase {
public:
	ActionLed(int gpio, int locoId, int func);
	virtual ~ActionLed();

	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(int status);
	// Reports just the changed Bit
	virtual void DCCFunc(int id, int bit, int newvalue);
private:
	int locoId;
	int func;
	int gpio;

};

#endif /* ACTIONLED_H_ */

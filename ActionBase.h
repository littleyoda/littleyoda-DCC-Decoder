/*
 * ActionBase.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef ACTIONBASE_H_
#define ACTIONBASE_H_
#include <Arduino.h>
#include "interfaceLoop.h"

class ActionBase: public interfaceLoop {
public:
	ActionBase();
	virtual ~ActionBase();
	//virtual int loop();
	virtual void TurnoutCmd(int id, int direction, int source);

	// Reports the Loco Speed and Direction
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);

	// Reports just the changed Bit
	virtual void DCCFunc(int id, int bit, int newvalue, int source);

	// Reports the Status of the all functions (bit 0 => F0, bit 1 => F1, ...)
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);

	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);

	// Calls from the Webserver for post and get requests
	virtual void setSettings(String key, String value);

};

#endif /* ACTIONBASE_H_ */

/*
 * ActionBase.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef ACTIONBASE_H_
#define ACTIONBASE_H_
#include <Arduino.h>

class ActionBase {
public:
	ActionBase();
	virtual ~ActionBase();
	virtual void loop();
	virtual void TurnoutCmd(int id, int direction);
	// Reports the Loco Speed and Direction
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps);
	// Reports just the changed Bit
	virtual void DCCFunc(int id, int bit, int newvalue);
	// Reports the Status of the all functions (bit 0 => F0, bit 1 => F1, ...)
	virtual void DCCFunc(int id, unsigned long int newvalue);
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(int status);

};

#endif /* ACTIONBASE_H_ */

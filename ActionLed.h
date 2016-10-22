/*
 * ActionLed.h
 *
 *  Created on: 27.08.2016
 *      Author: sven
 */

#ifndef ACTIONLED_H_
#define ACTIONLED_H_

#include <Arduino.h>
#include <LinkedList.h>
#include "ActionBase.h"

class ActionLed: public ActionBase {
public:
	ActionLed(int gpio, int locoId, int func);
	virtual ~ActionLed();
	virtual void setPattern(const char* pattern);
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
	virtual void setSettings(int status);
	virtual int loop();

	// Reports just the changed Bit
	virtual void DCCFunc(int id, int bit, int newvalue, int source);
private:
	int locoId;
	int func;
	int gpio;
	int currentStatus = 0;
	LinkedList<int> pattern = LinkedList<int>();
	int patternPos = 0;

};

#endif /* ACTIONLED_H_ */

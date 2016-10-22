/*
 * ActionTesting.h
 *
 *  Created on: 04.09.2016
 *      Author: sven
 */

#ifndef ACTIONTESTING_H_
#define ACTIONTESTING_H_

#include "ActionBase.h"

class ActionTesting: public ActionBase {
public:
public:
	ActionTesting(int gpio);
	virtual ~ActionTesting();

	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
	virtual int loop();
private:
	int status = 2;
	int gpio;
	unsigned long last = 0;
};

#endif /* ACTIONTESTING_H_ */

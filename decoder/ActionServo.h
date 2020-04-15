/*
 * ActionServo.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef ACTIONSERVO_H_
#define ACTIONSERVO_H_

#include "INotify.h"
#include "ISettings.h"

class ActionServo: public ILoop, public ISettings {
public:
	ActionServo(int pin);
	virtual ~ActionServo();

	virtual int loop();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
private:
	int pin;
	int id;
};

#endif /* ACTIONSERVO_H_ */

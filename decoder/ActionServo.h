/*
 * ActionServo.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef ACTIONSERVO_H_
#define ACTIONSERVO_H_

#include "ISettings.h"
#include "ILoop.h"

class ActionServo: public ILoop, public ISettings {
public:
	ActionServo(int pin);
	virtual ~ActionServo();

	virtual int loop()  override;
	virtual String getHTMLCfg(String urlprefix) override;
	virtual String getHTMLController(String urlprefix) override;
	virtual void setSettings(String key, String value) override;
private:
	int pin;
	int id;
};

#endif /* ACTIONSERVO_H_ */

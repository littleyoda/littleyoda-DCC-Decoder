/*
 * ActionServo.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef ACTIONSERVO_H_
#define ACTIONSERVO_H_

#include <Servo.h>
#include "ActionBase.h"

class ActionServo: public ActionBase {
public:
	ActionServo(int pin, int id);
	virtual ~ActionServo();

	virtual void loop();
	virtual void TurnoutCmd(int id, int direction);
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps);
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
	virtual void setSettings(int status);
private:
	int pin;
	int id;
	unsigned long lastcommand = 0;
	Servo myservo;bool isAttach = false;
	void attach();
	void detach();

};

#endif /* ACTIONSERVO_H_ */

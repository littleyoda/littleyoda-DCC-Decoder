/*
 * ActionStepperOutput.h
 *
 *  Created on: 23.01.2019
 *      Author: sven
 */

#ifndef ACTIONSTEPPEROUTPUT_H_
#define ACTIONSTEPPEROUTPUT_H_

#include "ISettings.h"
#include "Pin.h"
#include "INotify.h"
#include "ISettings.h"
#include "GPIO.h"

class ActionStepperOutput: public ISettings, public ILoop  {
public:
	ActionStepperOutput(Pin* p1, Pin* p2, Pin* p3, Pin* p4);
	virtual ~ActionStepperOutput();
	virtual int loop();
	virtual void setSettings(String key, String value);
	virtual String getHTMLController(String urlprefix);

private:
		int state = 0;
		int range = 4000;
		long current = 0;
		long target = 0;
		Pin** pins = new Pin*[4];


		byte steps[8][4] =
		{
		{ 0, 0, 0, 1 },
		{ 0, 0, 1, 1 },
		{ 0, 0, 1, 0 },
		{ 0, 1, 1, 0 },
		{ 0, 1, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 0, 0, 1 },
		} ;
};

#endif /* ACTIONSTEPPEROUTPUT_H_ */

/*
 * ActionBase.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef INOTIFY_H_
#define INOTIFY_H_
#include <Arduino.h>
#include <LinkedList.h>

#include "ILoop.h"

class INotify {
public:
	INotify();
	virtual ~INotify();
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

	struct requestInfo {
		enum ART {TURNOUT, LOCO } art;
		unsigned int id;
	};
	LinkedList<requestInfo*>* getRequestList();
protected:
    LinkedList<requestInfo*> requestList = LinkedList<requestInfo*>();
};

#endif /* INOTIFY_H_ */

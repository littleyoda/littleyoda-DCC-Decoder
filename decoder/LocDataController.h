/*
 * LOCDATACONTROLLER.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef LOCDATACONTROLLER_H_
#define LOCDATACONTROLLER_H_

#include "ISettings.h"
#include "Controller.h"

class LocDataController: public ISettings {
public:
	LocDataController(Controller* c, LinkedList<int> *loclist, LinkedList<int> *tolist);
	virtual ~LocDataController();
	virtual void setSettings(String key, String value);
	void requestLocData();
	void requestTurnoutData();
	void getInternalStatus(IInternalStatusCallback* cb, String key);
	
private:
	Controller* controller;
	int currentADDR = 1;
	int currentIdx = 1;
	LocData* locdata;
	LinkedList<int> *addrlist;

	int currentTurnOutAddr = 1;
	int currentTurnOutIdx = 1;
	TurnOutData* turnoutdata;
	LinkedList<int> *turnoutaddrlist;
};

#endif /* LOCDATACONTROLLER_H_ */

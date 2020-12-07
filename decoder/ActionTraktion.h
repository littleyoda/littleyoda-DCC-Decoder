/*
 * ACTIONTRAKTION.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef ACTIONTRAKTION_H_
#define ACTIONTRAKTION_H_

#include "INotify.h"

class Controller;
class ActionTraktion : public INotify{
public:
	ActionTraktion(Controller* c, int n, int t, int v, int f);
	virtual ~ActionTraktion();
	virtual void DCCSpeed(int id, int speed, int direction, int source) override;
	virtual void DCCFunc(int id, int bit, int newvalue, int source) override;
	virtual void getRequestList(LinkedList<requestInfo*>* list) override;
private:
	int normalAddr;
	int normalTraktion;
	int virtuelAddr;
	int func;
	Controller* controller;
	bool isNormal;
	requestInfo* r2;
};

#endif /* ACTIONTRAKTION_H_ */

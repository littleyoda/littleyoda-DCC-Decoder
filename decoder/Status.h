/*
 * STATUS.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "IStatus.h"
class Controller;

class Status : public IStatus {
public:
	Status(Controller* c);
	virtual ~Status();
	virtual void setName(String s);
	virtual String getName();
	virtual String getInternalStatus(String key);

private:
	Controller* controller;
};

#endif /* STATUS_H_ */

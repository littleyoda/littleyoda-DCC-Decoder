/*
 * ISTATUS.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef ISTATUS_H_
#define ISTATUS_H_

#include "Arduino.h"

class IStatus {
public:
	IStatus();
	virtual ~IStatus();
	virtual void setName(String s);
	virtual String getName();
	virtual String getInternalStatus(String key);
protected:
	String* id;

};

#endif /* ISTATUS_H_ */

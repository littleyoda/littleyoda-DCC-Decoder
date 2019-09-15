/*
 * IINTERNALSTAUTSCALLBACK.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef IINTERNALSTAUTSCALLBACK_H_
#define IINTERNALSTAUTSCALLBACK_H_
#include "Arduino.h"

class IInternalStatusCallback {
public:
	IInternalStatusCallback();
	virtual ~IInternalStatusCallback();
	virtual	void send(String modul, String key, String value);
	virtual void reset() {};
};

#endif /* IINTERNALSTAUTSCALLBACK_H_ */

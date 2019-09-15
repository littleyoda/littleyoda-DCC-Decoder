/*
 * INTERNALSTATUSASSTRING.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INTERNALSTATUSASSTRING_H_
#define INTERNALSTATUSASSTRING_H_

#include "IInternalStatusCallback.h"

class InternalStatusAsString : public IInternalStatusCallback {
public:
	InternalStatusAsString();
	virtual ~InternalStatusAsString();
	virtual void send(String modul, String key, String value);
	String get();
	virtual void reset();
private:
	String value;
};

#endif /* INTERNALSTATUSASSTRING_H_ */

/*
 * INTERNALSTATUSASJSON.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INTERNALSTATUSASJSON_H_
#define INTERNALSTATUSASJSON_H_

#include "IInternalStatusCallback.h"
#include "ArduinoJson.h"

class InternalStatusAsJson : public IInternalStatusCallback {
public:
	InternalStatusAsJson();
	virtual ~InternalStatusAsJson();

	virtual void send(String modul, String key, String value);
	virtual void reset();
	virtual String get();
	virtual void print();
private:
	DynamicJsonDocument* doc;

};

#endif /* INTERNALSTATUSASJSON_H_ */

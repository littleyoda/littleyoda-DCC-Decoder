/*
 * INTERNALSTATUSTOFILE.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INTERNALSTATUSTOFILE_H_
#define INTERNALSTATUSTOFILE_H_

#include "Arduino.h"
#include "IInternalStatusCallback.h"
#include <FS.h>
#ifdef ESP32
	#include "SPIFFS.h"
#endif

class InternalStatusToFile : public IInternalStatusCallback {
public:
	InternalStatusToFile();
	virtual ~InternalStatusToFile();
	virtual void send(String modul, String key, String value);
	void setFile(File* f);
private:
	File* file;
};

#endif /* INTERNALSTATUSTOFILE_H_ */

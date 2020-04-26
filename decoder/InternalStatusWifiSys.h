/*
 * INTERNALSTATUSWIFISYS.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INTERNALSTATUSWIFISYS_H_
#define INTERNALSTATUSWIFISYS_H_

#include "IInternalStatusCallback.h"

class InternalStatusWifiSys {
public:
	InternalStatusWifiSys();
	virtual ~InternalStatusWifiSys();
	static void getInternalStatus(IInternalStatusCallback* cb, String modul, String key);
	static void handleRequest(String key, String value);
};

#endif /* INTERNALSTATUSWIFISYS_H_ */

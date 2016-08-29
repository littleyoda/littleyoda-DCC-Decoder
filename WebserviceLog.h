/*
  * WebserviceLog.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef WEBSERVICELOG_H_
#define WEBSERVICELOG_H_

#include "Arduino.h"
#include "WebserviceBase.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class WebserviceLog: public WebserviceBase {
public:
	WebserviceLog();
	virtual ~WebserviceLog();
	virtual char const* getUri();
	virtual void run();
};

#endif /* WEBSERVICELOG_H_ */

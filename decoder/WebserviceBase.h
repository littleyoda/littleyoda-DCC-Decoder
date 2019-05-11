/*
 * WebserviceBase.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef WEBSERVICEBASE_H_
#define WEBSERVICEBASE_H_
#include "Arduino.h"

#ifdef ESP8266
	#include <ESP8266WebServer.h>
#elif ESP32
	#include <WebServer.h>
#endif

//
//#ifdef ESP8266
//	class ESP8266WebServer;
//#elif ESP32
//	class WebServer;
//#endif

class WebserviceBase {
public:
	WebserviceBase();
	virtual ~WebserviceBase();
	virtual char const* getUri() = 0;
	virtual void run() = 0;
#ifdef ESP8266
	void setServer(ESP8266WebServer* server);
#elif ESP32
	void setServer(WebServer* server);
#endif

	virtual String  getLinkText();


protected:
#ifdef ESP8266
	ESP8266WebServer* server;
#elif ESP32
	WebServer* server;
#endif

	void sendBasicHeader();
	void finishSend();
	void send(const String& content);

};

#endif /* WEBSERVICEBASE_H_ */

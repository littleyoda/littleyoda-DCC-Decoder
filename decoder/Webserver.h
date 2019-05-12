/*
 * Webserver.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef WEBSERVER_H_
#define WEBSERVER_H_

#include <LinkedList.h>
#include <WiFiClient.h>
#ifdef ESP8266
	#include <ESP8266WebServer.h>
	#include <ESP8266HTTPUpdateServer.h>
	#include <ESP8266mDNS.h>
#elif ESP32
	#include <WebServer.h>
	#include <ESPmDNS.h>
	#include <Update.h>
#else
	#error "This Arch is not supported"
#endif

#include <FS.h>
#include "WebserviceBase.h"
#include "ILoop.h"


class Controller;

class Webserver: public ILoop {
public:
	Webserver(Controller* c);
	virtual int loop();
	virtual ~Webserver();
#ifdef ESP8266
	static ESP8266WebServer* server;
#elif ESP32
	static WebServer* server;
#endif
	void registerWebServices(WebserviceBase* base);


private:
	bool loadFromSPIFFS(String filepath);
	void handleNotFound();
	void handleRoot();
	void handleDel();
	void handleController();
	void handleVersion();
	void handleCfg();
	void handleSet();
	void handleFilelist();
	void handleUpload();
	void handleDoFormat();
	void handleFormat();
	int lastWifiStatus = 6;
	File fsUploadFile;
	Controller* controll;
	LinkedList<WebserviceBase*> services = LinkedList<WebserviceBase*>();
	#ifdef ESP8266
		static ESP8266HTTPUpdateServer* httpUpdater;
	#endif
};


#endif /* WEBSERVER_H_ */

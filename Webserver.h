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
#include <ESP8266WebServer.h>

#include "interfaceLoop.h"
#include "WebserviceBase.h"

class Controller;

class Webserver: public interfaceLoop {
public:
	Webserver(Controller* c);
	virtual void loop();
	virtual ~Webserver();
	static ESP8266WebServer* server;
	void addServices(WebserviceBase* base);

private:
	bool loadFromSPIFFS(String filepath);
	void handleNotFound();
	void handleRoot();
	void handleController();
	void handleSet();
	int lastWifiStatus = 6;

	Controller* controll;

};

#endif /* WEBSERVER_H_ */

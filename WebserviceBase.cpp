/*
 * WebserviceBase.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */
#include "Arduino.h"
#include "WebserviceBase.h"

WebserviceBase::WebserviceBase() {
	server = NULL;
}

WebserviceBase::~WebserviceBase() {
}

void WebserviceBase::setServer(ESP8266WebServer* server) {
	this->server = server;
}

String  WebserviceBase::getLinkText() {
	return "";
}

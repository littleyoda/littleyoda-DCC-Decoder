/*
 * WebserviceBase.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include "WebserviceBase.h"

WebserviceBase::WebserviceBase() {

}

WebserviceBase::~WebserviceBase() {
}

void WebserviceBase::setServer(ESP8266WebServer* server) {
	this->server = server;
}

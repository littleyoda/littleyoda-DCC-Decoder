/*
 * WebserviceBase.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */
#include "Arduino.h"
#include "WebserviceBase.h"
#ifdef ESP8266
	#include <ESP8266WebServer.h>
#elif ESP32
	#include <WebServer.h>
#endif



WebserviceBase::WebserviceBase() {
	server = NULL;
}

WebserviceBase::~WebserviceBase() {
}

#ifdef ESP8266
void WebserviceBase::setServer(ESP8266WebServer* server) {
	this->server = server;
}
#elif ESP32
void WebserviceBase::setServer(WebServer* server) {
	this->server = server;
}
#endif


String  WebserviceBase::getLinkText() {
	return "";
}

void WebserviceBase::sendBasicHeader() {
    send(
    		"HTTP/1.1 200 OK\r\n"
    		"Content-Type: text/html\r\n"
    		"Connection: close\r\n"
    		"Access-Control-Allow-Origin: *\r\n"
    		"\r\n"
    	);
}

void WebserviceBase::finishSend() {
	server->client().stop();
}

void WebserviceBase::send(const String& content) {
	server->sendContent(content);
}

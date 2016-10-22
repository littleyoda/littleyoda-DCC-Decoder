/*
 * WebserviceBase.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */
#include "Arduino.h"
#include "WebserviceBase.h"
#include <ESP8266WebServer.h>


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

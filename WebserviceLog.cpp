/*
 * WebserviceLog.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include "WebserviceLog.h"
#include "Logger.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "Utils.h"

WebserviceLog::WebserviceLog() {
}

WebserviceLog::~WebserviceLog() {
}

char const* WebserviceLog::getUri() {
	return "/log";
}

void WebserviceLog::run() {
	String message =
			"<html><body><table border=\"1\"><tr><th>Attribut</th><th>Status</th></tr>";
	message += "<tr><td>Memory</td><td>" + String(ESP.getFreeHeap()) + "/"
			+ String(Logger::getInstance()->startmemory) + " Bytes </td></tr>";
	message += "<tr><td>Uptime</td><td>" + String(millis() / 1000)
			+ " sek</td></tr>";
	message += "<tr><td>Wifi RSSI</td><td>" + String(WiFi.RSSI())
			+ "</td></tr>";
	message += "<tr><td>Wifi Status</td><td>"
			+ Utils::wifi2String(WiFi.status()) + "</td></tr>";
	message += "<tr><td>Wifi SSID</td><td>" + WiFi.SSID() + "</td></tr>";
	message += Logger::getInstance()->logsToString();
	message += "</body></html>";
	server->send(200, "text/html", message);

}

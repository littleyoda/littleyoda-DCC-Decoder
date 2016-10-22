/*
 * WebserviceLog.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include "WebserviceLog.h"
#include "Logger.h"
#include "Webserver.h"
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
	sendBasicHeader();
	send(Utils::getHTMLHeader());
	send("<table border=\"1\"><tr><th>Attribut</th><th>Status</th></tr>\n");
	send("<tr><td>Memory</td><td>" + String(ESP.getFreeHeap()) + "/"
			+ String(Logger::getInstance()->startmemory) + " Bytes </td></tr>\n");
	send("<tr><td>Uptime</td><td>" + String(millis() / 1000)
			+ " sek</td></tr>\n");
	send("<tr><td>Wifi RSSI</td><td>" + String(WiFi.RSSI())
			+ "</td></tr>\n");
	send("<tr><td>Wifi Status</td><td>"
			+ Utils::wifi2String(WiFi.status()) + "</td></tr>\n");
	send("<tr><td>Wifi SSID</td><td>" + WiFi.SSID() + "</td></tr>");
	LinkedList<String>* list = Logger::getInstance()->getLogs();
	for (int i = 0; i < list->size(); i++) {
		send("<tr><td>Log ");
		send(String(i));
		send("</td><td>");
		send(list->get(i));
		send("</td></tr>\n");
	}
	send("</table>");
	send(Utils::getHTMLFooter());
	finishSend();
}

String  WebserviceLog::getLinkText() {
	return "&#128214;";
}


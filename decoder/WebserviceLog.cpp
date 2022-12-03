/*
 * WebserviceLog.cpp
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#include "WebserviceLog.h"
#include "Logger.h"
#include "WebserverA.h"
#include <WiFiClient.h>
#include "Utils.h"

WebserviceLog::WebserviceLog() {
}

WebserviceLog::~WebserviceLog() {
}

char const* WebserviceLog::getUri() {
	return "/log";
}

void WebserviceLog::run() {
	String end = F("</td></tr>\n");
	String start = F("<tr><td>");
	sendBasicHeader();
	send(Utils::getHTMLHeader());
	send("<table border=\"1\"><tr><th>Attribut</th><th>Status</th></tr>\n");
	send(start + "Memory (free/free at startup)</td><td>" + String(ESP.getFreeHeap()) + "/"
			+ String(Logger::getInstance()->startmemory) + " Bytes" + end);
	send(start + "Uptime</td><td>" + String(millis() / 1000)
			+ " sek" + end);
	send(start + "Version</td><td>" + String(compile_date)
			+ end);
	send(start + "Wifi RSSI</td><td>" + String(WiFi.RSSI())
			+ end);
	send(start + "Wifi Status</td><td>"
			+ Utils::wifi2String(WiFi.status()) + end);
	send(start + "Wifi SSID</td><td>" + WiFi.SSID() + end);
	send(start + "IP (Access Point)</td><td>" + WiFi.softAPIP().toString() + end);
	send(start + "IP (WLAN Client)</td><td>" + WiFi.localIP().toString() + end);
	send(start + "Pins</td><td>");
	send(GPIOobj.getUsage("<br/>"));
	send(end);
	LinkedList<Logger::logdata*>* list = Logger::getInstance()->getLogs();
	for (int i = 0; i < list->size(); i++) {
		send(start);
		send(F("Log "));
		send(String(i));
		send("</td><td>");
		send(list->get(i)->msg);
		send(end);
	}
	send("</table>");
	send(Utils::getHTMLFooter());
	finishSend();
}

String  WebserviceLog::getLinkText() {
	return "&#128214;";
}


/*
 * WebserviceDCCLogger.cpp
 *
 *  Created on: 07.09.2016
 *      Author: sven
 */

#include "WebserviceCommandLogger.h"

#include <ESP8266WebServer.h>
#include "Arduino.h"
#include "Utils.h"

WebserviceCommandLogger::WebserviceCommandLogger() {
}

WebserviceCommandLogger::~WebserviceCommandLogger() {
}

int WebserviceCommandLogger::loop() {
	return -1;
}

void WebserviceCommandLogger::TurnoutCmd(int id, int direction, int source) {
	addToLog("Turnout</td><td>" + String(id) + "</td><td>" + String(direction) + "</td><td>" + String(source));
}

void WebserviceCommandLogger::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
	addToLog("Speed</td><td>" + String(id) + "</td><td>" + String(direction) + "/" + String(speed) + "/" + String(SpeedSteps) + "</td><td>" + String(source));
}


void WebserviceCommandLogger::DCCFunc(int id, unsigned long int newvalue, int source) {
	addToLog("Func</td><td>" + String(id) + "</td><td>" + String(newvalue, BIN) + "</td><td>" + String(source));
}

String WebserviceCommandLogger::getHTMLCfg(String urlprefix) {
	return "";
}

String WebserviceCommandLogger::getHTMLController(String urlprefix) {
	return "";
}

void WebserviceCommandLogger::setSettings(String key, String value) {
}

void WebserviceCommandLogger::addToLog(String s) {
	Serial.println(s);
	logger.add(String(millis() / 1000) + "</td><td>" + s);
	if (logger.size() > 100) {
		logger.shift();
	}
}

const char* WebserviceCommandLogger::getUri() {
	return "/dcclogger";
}

void WebserviceCommandLogger::run() {
	String message = Utils::getHTMLHeader();
	message += "<table>"
			"<thead>"
			"<tr><th>Zeitpunkt</th><th></th><th>ID</th><th>Wert</th><th>Quelle (0=DCC, 1=WLAN)</th>"
		    "</thead><tbody>";
	for (int i = 0; i < logger.size(); i++) {
		message += "<tr><td>";
		message += String(logger.get(i));
		message += "</td></tr>\n";
	}
	message += "</tbody></table>";
	message += Utils::getHTMLFooter();
	server->send(200, "text/html", message);
}

String WebserviceCommandLogger::getLinkText() {
	return "&#x2315;";
}

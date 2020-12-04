/*
 * WebserviceDCCLogger.cpp
 *
 *  Created on: 07.09.2016
 *      Author: sven
 */

#include "WebserviceCommandLogger.h"

#include "Arduino.h"
#include "Utils.h"

WebserviceCommandLogger::WebserviceCommandLogger() {
}

WebserviceCommandLogger::~WebserviceCommandLogger() {
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
//	Serial.println(s);
	logger.add(String(millis() / 1000) + "</td><td>" + s);
	if (logger.size() > maxLog) {
		logger.shift();
	}
}

const char* WebserviceCommandLogger::getUri() {
	return "/dcclogger";
}

void WebserviceCommandLogger::run() {
	sendBasicHeader();
	send(Utils::getHTMLHeader());
	send("<table>"
			"<thead>"
			"<tr><th>Zeitpunkt</th><th></th><th>ID</th><th>Wert</th><th>Quelle (0=DCC, 1=WLAN, 2=Internal, 3=Z21Server, 4=Rocrail )</th>"
		    "</thead><tbody>");
	for (int i = 0; i < logger.size(); i++) {
		send("<tr><td>");
		send(logger.get(i));
		send("</td></tr>\n");
	}
	send("</tbody></table>");
	send(Utils::getHTMLFooter());
	finishSend();
}

String WebserviceCommandLogger::getLinkText() {
	return "&#x2315;";
}

unsigned int WebserviceCommandLogger::getMemUsage() {
	unsigned long usage = 0;
	for (int i = 0; i < logger.size(); i++) {
		usage +=  logger.get(i).length();
	}
	return usage;
}

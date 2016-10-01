/*
 * WebserviceDCCSniffer.cpp
 *
 *  Created on: 08.09.2016
 *      Author: sven
 */

#include "WebserviceDCCSniffer.h"
#include "Logger.h"
#include "Utils.h"
#include "NmraDcc2.h"
#include <ESP8266WebServer.h>


void notifyDccMsg(DCC_MSG * Msg) {
	if (WebserviceDCCSniffer::_instance != NULL) {
		String s = "";
		for (int i = 0; i < Msg->Size; i++) {
			String t = String(Msg->Data[i], HEX);
			if (t.length() == 1) {
				t = "0" + t;
			}
			s += " " + t;
		}
		s += "</td><td>";
		for (int i = 0; i < Msg->Size; i++) {
			String t = String(Msg->Data[i], BIN);
			while (t.length() < 8) {
				t = "0" + t;
			}
			s += " " + t;
		}
		WebserviceDCCSniffer::_instance->addToLog(s);
	}
}

WebserviceDCCSniffer *WebserviceDCCSniffer::_instance = NULL;

WebserviceDCCSniffer::WebserviceDCCSniffer() {
	WebserviceDCCSniffer::_instance = this;
}

WebserviceDCCSniffer::~WebserviceDCCSniffer() {
}

String WebserviceDCCSniffer::getHTMLCfg(String urlprefix) {
	return "";
}

String WebserviceDCCSniffer::getHTMLController(String urlprefix) {
	return "";
}

int WebserviceDCCSniffer::loop() {
	return -1;
}

void WebserviceDCCSniffer::addToLog(String s) {
	//Serial.println(s);
	logger.add(String(millis() / 1000) + "</td><td>" + s);
	if (logger.size() > 100) {
		logger.shift();
	}
}

const char* WebserviceDCCSniffer::getUri() {
	return "/dccsniffer";
}

void WebserviceDCCSniffer::run() {
	String message = Utils::getHTMLHeader();
	message += "Die letzten " + String(logger.size()) + " empfangenen DCC-Pakete:<br><table>"
			"<thead>"
			"<tr><th>Zeitpunkt</th><th>Wert</th><th>Wert</th>"
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

String WebserviceDCCSniffer::getLinkText() {
	return "&#x1F52C;";
}

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


void notifyDccMsg(DCC_MSG * Msg) {
	if (WebserviceDCCSniffer::_instance != NULL) {
		String s;
		s.reserve(Msg->Size*(3+9) + 9 + 1);
		for (int i = 0; i < Msg->Size; i++) {
			String t = String(Msg->Data[i], HEX);
			s.concat(Utils::repeatString("0", 2 - t.length()));
			s.concat(t);
			s.concat(" ");
		}
		s.concat("</td><td>");
		for (int i = 0; i < Msg->Size; i++) {
			String t = String(Msg->Data[i], BIN);
			s.concat(Utils::repeatString("0", 8 - t.length()));
			s.concat(t);
			s.concat(" ");
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


void WebserviceDCCSniffer::addToLog(String s) {
	//Serial.println(s);
	logger.add(String(millis() / 1000) + "</td><td>" + s);
	if (logger.size() > maxLog) {
		logger.shift();
	}
}

const char* WebserviceDCCSniffer::getUri() {
	return "/dccsniffer";
}

void WebserviceDCCSniffer::run() {
	sendBasicHeader();
	String message = Utils::getHTMLHeader();
	message += "Die letzten " + String(logger.size()) + " empfangenen DCC-Pakete:<br><table>"
			"<thead>"
			"<tr><th>Zeitpunkt</th><th>Wert</th><th>Wert</th>"
		    "</thead><tbody>";
	send(message);
	for (int i = 0; i < logger.size(); i++) {
		send("<tr><td>" + String(logger.get(i)) + "</td></tr>\n");
	}
	send("</tbody></table>" + Utils::getHTMLFooter());
	finishSend();
}

String WebserviceDCCSniffer::getLinkText() {
	return "&#x1F52C;";
}

unsigned int WebserviceDCCSniffer::getMemUsage() {
	unsigned long usage = 0;
	for (int i = 0; i < logger.size(); i++) {
		usage +=  logger.get(i).length();
	}
	return usage;
}

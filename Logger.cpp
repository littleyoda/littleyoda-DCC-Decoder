/*
 * Logger.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include "Logger.h"


Logger::Logger() {
	startmemory = ESP.getFreeHeap();
}

Logger* Logger::getInstance() {
	if (theInstance == NULL) {
		theInstance = new Logger();
	}
	return theInstance;
}

Logger::~Logger() {
}

void Logger::addToLog(String s) {
	logdata* log = new logdata;
	log->send = false;
	log->msg = String(millis() / 1000) + String(": ") + s;
	logger.add(log);

	if (logger.size() > maxLog) {
		logdata* l = logger.shift();
		delete(l);
	}
}

int Logger::findLastUnsend() {
	int last = -1;
	for (int idx = logger.size() - 1; idx >= 0; idx--) {
		logdata* l = logger.get(idx);
		if (l->send) {
			break;
		}
		last = idx;
	}
	return last;
}

int Logger::loop() {
	if (udp == NULL) {
		return 4000;
	}
	int last = findLastUnsend();
	if (last == -1) {
		return 500;
	}
	logdata* log = logger.get(last);
	udp->beginPacket(*logserver, 514);
	udp->write(log->msg.c_str(), log->msg.length());
	if (udp->endPacket() == 1) {
		log->send = true;
	} else {
		return 500; // Waiting for connection
	}
	return 10;
}

LinkedList<Logger::logdata*>* Logger::getLogs() {
	return &logger;
}



unsigned int Logger::getMemUsage() {
	unsigned int usage = 0;
	for (int i = 0; i < logger.size(); i++) {
		usage +=  logger.get(i)->msg.length();
	}
	return usage;
}

void Logger::setIPAddress(IPAddress* ip) {
	logserver = ip;
	udp = new WiFiUDP();
	udp->begin(514);
}

Logger* Logger::theInstance = NULL;

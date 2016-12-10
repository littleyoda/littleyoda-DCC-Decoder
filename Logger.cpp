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
	Serial.println(s);
	logger.add(String(millis() / 1000) + String(": ") + s);
	if (logger.size() > maxLog) {
		logger.shift();
	}
}

LinkedList<String>* Logger::getLogs() {
	return &logger;
}

Logger* Logger::theInstance = NULL;


unsigned int Logger::getMemUsage() {
	unsigned int usage = 0;
	for (int i = 0; i < logger.size(); i++) {
		usage +=  logger.get(i).length();
	}
	return usage;
}

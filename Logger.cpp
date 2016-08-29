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
	logger.add(String(millis() / 1000) + ": " + s);
	if (logger.size() > 100) {
		logger.shift();
	}
}

LinkedList<String> Logger::getLogs() {
	return logger;
}

Logger* Logger::theInstance = NULL;

String Logger::logsToString() {
	String message = "";
	for (int i = 0; i < logger.size(); i++) {
		message += "<tr><td>Log ";
		message += String(i);
		message += "</td><td>";
		message += String(logger.get(i));
		message += "</td></tr>\n";
	}
	return message;
}

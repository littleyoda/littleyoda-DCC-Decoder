/*
 * Logger.cpp
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#include "Logger.h"



Logger::Logger() {
	startmemory = ESP.getFreeHeap();
	udp = nullptr;
	logserver = nullptr;
	#ifdef ESP8266
	Debug.begin("xxx"); // Initialize the WiFi server
	Debug.setResetCmdEnabled(true); // Enable the reset command
	Debug.showProfiler(false); // Profiler (Good to measure times, to optimize codes)
	Debug.showColors(true); // Colors
	Debug.setSerialEnabled(false);
	#endif
}

Logger* Logger::getInstance() {
	if (theInstance == nullptr) {
		theInstance = new Logger();
	}
	return theInstance;
}

Logger::~Logger() {
}

void Logger::addToLog(LogLevel loglevel, String s) {
	//TODO loglevel check
	Serial.println(s);
	if (maxLog == 0) {
		return;
	}
	logdata* log = new logdata;
	log->send = false;
	log->msg = String(millis() / 1000) + String(": ") + s;
	log->sendRD = false;
	logger.add(log);
	if (logger.size() > maxLog) {
		logdata* l = logger.shift();
		delete(l);
	}
	// Remove Log-Messages if free memory is low
	while ((logger.size() > 0) && (ESP.getFreeHeap() < 4000)) {
		logdata* l = logger.shift();
		delete(l);
	}
}

void Logger::printf(LogLevel loglevel, const char * msg, ...) {
	va_list arg;
	va_start(arg, msg);
	char temp[100];
	char* buffer = temp;
	size_t len = vsnprintf_P(temp, sizeof(temp), msg, arg);
	va_end(arg);
	if (len > sizeof(temp) - 1) {
		buffer = new char[len + 1];
		if (!buffer) {
			return;
		}
		va_start(arg, msg);
		vsnprintf_P(buffer, len + 1, msg, arg);
		va_end(arg);
	}
	String out = String(buffer);
	if (buffer != temp) {
		delete[] buffer;
	}
	addToLog(loglevel, out);
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

int Logger::findLastUnsendRD() {
	int last = -1;
	for (int idx = logger.size() - 1; idx >= 0; idx--) {
		logdata* l = logger.get(idx);
		if (l->sendRD) {
			break;
		}
		last = idx;
	}
	return last;
}

int Logger::loop() {
	#ifdef ESP8266
	Debug.handle();
	#endif
	boolean finish = true;
	if (!sendUDP()) {
		finish = false;
	}
	if (!sendRD()) {
		finish = false;
	}
	return (finish ? 100 : 10);
}

boolean Logger::sendRD() {
	#ifdef ESP8266
	if (!Debug.isActive(RemoteDebug::ANY)) {
		return true;
	}
	#endif
	int last = findLastUnsendRD();
	if (last == -1) {
		return true;
	}
	logdata* log = logger.get(last);
	log->sendRD = true;
	Serial.println("Sending: " + log->msg);
	#ifdef ESP8266
	Debug.println(log->msg.c_str());
	#endif
	return false;
}

boolean Logger::sendUDP() {
	if (udp == nullptr) {
		return true;
	}
	int last = findLastUnsend();
	if (last == -1) {
		return true;
	}
	logdata* log = logger.get(last);
	udp->beginPacket(*logserver, 5514);
	#ifdef ESP8266
		size_t ret = udp->write(log->msg.c_str(), log->msg.length());
	#elif ESP32
		size_t ret = udp->write((const uint8_t *)log->msg.c_str(), log->msg.length());
	#else
		#error "This Arch is not supported"
	#endif
	if (udp->endPacket() == 1) {
		log->send = true;
	} 
	return false;
}

LinkedList<Logger::logdata*>* Logger::getLogs() {
	return &logger;
}

void Logger::resendAll() {
	for (int idx = logger.size() - 1; idx >= 0; idx--) {
		logdata* l = logger.get(idx);
		l->send = false;
	}
}

unsigned int Logger::getMemUsage() {
	unsigned int usage = 0;
	for (int i = 0; i < logger.size(); i++) {
		usage +=  (logger.get(i)->msg.length() + sizeof(String));
	}
	return usage;
}

void Logger::setIPAddress(IPAddress* ip) {
	logserver = ip;
	udp = new WiFiUDP();
	udp->begin(514);
}

Logger* Logger::theInstance = nullptr;

void Logger::log(LogLevel loglevel, String s) {
	getInstance()->addToLog(loglevel, s);
}

void Logger::log(LogLevel loglevel, String modul, String s) {
	getInstance()->addToLog(loglevel, "[" + modul + "] " + s);
}

void Logger::changeLogLevel(int diff) {
	#ifdef ESP8266
	getInstance()->Debug.setLogLevel(getInstance()->Debug.getLogLevel() + diff);
	String out = "";
	switch (getInstance()->Debug.getLogLevel()) {
		case LogLevel::TRACE: 
			out = "TRACE";
			break;
		case LogLevel::DEBUG:
			out = "DEBUG";
			break;
		case LogLevel::INFO:
			out = "INFO";
			break;
		case LogLevel::WARNING:
			out = "WARNING";
			break;
		case LogLevel::ERROR:
			out = "ERROR";
			break;
		default:
			out = "UNKNOWN";
	}
	log(LogLevel::ERROR, "Loglevel: " + out);
	#endif
}

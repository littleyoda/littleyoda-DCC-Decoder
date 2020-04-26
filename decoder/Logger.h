/*
 * Logger.h
 *
 *  Created on: 20.08.2016
 *      Author: sven
 */

#ifndef LOGGER_H_
#define LOGGER_H_
#include <LinkedList.h>
#include <Arduino.h>
#include <WiFiUdp.h>
#include "ILoop.h"
#include "RemoteDebug.h" 

enum LogLevel {
	TRACE = 1,
	DEBUG = 2,
	INFO = 3,
	WARNING = 4,
	ERROR = 5
};

class Logger : public ILoop  {
public:
	struct logdata { boolean send; boolean sendRD; String msg; };
	static Logger* getInstance();
	static void log(LogLevel loglevel, String s);
	static void changeLogLevel(int diff);
	void printf(LogLevel loglevel, const char * format, ...);
	void addToLog(LogLevel loglevel, String s);
	LinkedList<logdata*>* getLogs();
	int startmemory;
	int maxLog = 50;
	unsigned int getMemUsage();
	virtual int loop();
	void setIPAddress(IPAddress* ip);
	void resendAll();
	String format(const char * format, ...);

private:
	LinkedList<logdata*> logger = LinkedList<logdata*>();
	Logger();
	virtual ~Logger();
	WiFiUDP* udp;
	IPAddress* logserver;
	static Logger *theInstance;
	int findLastUnsend();
	int findLastUnsendRD();
	boolean sendUDP();
	boolean sendRD();

	RemoteDebug Debug;
};

#endif /* LOGGER_H_ */

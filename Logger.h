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
#include "interfaceLoop.h"

class Logger : public interfaceLoop  {
public:
	struct logdata { boolean send; String msg; };
	static Logger* getInstance();
	void addToLog(String s);
	LinkedList<logdata*>* getLogs();
	int startmemory;
	int maxLog = 50;
	unsigned int getMemUsage();
	virtual int loop();
	void setIPAddress(IPAddress* ip);

private:
	LinkedList<logdata*> logger = LinkedList<logdata*>();
	Logger();
	virtual ~Logger();
	WiFiUDP* udp;
	IPAddress* logserver;
	static Logger *theInstance;
	int findLastUnsend();
};

#endif /* LOGGER_H_ */

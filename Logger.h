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

class Logger {
public:
	static Logger* getInstance();
	void addToLog(String s);
	LinkedList<String>* getLogs();
	int startmemory;
	unsigned int getMemUsage();
private:
	LinkedList<String> logger = LinkedList<String>();
	Logger();
	virtual ~Logger();
	static Logger *theInstance;

};

#endif /* LOGGER_H_ */

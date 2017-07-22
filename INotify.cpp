/*
 * ActionBase.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "INotify.h"

INotify::INotify() {
}

INotify::~INotify() {
}

String INotify::getHTMLCfg(String urlprefix) {
	return "";
}

String INotify::getHTMLController(String urlprefix) {
	return "";
}


void INotify::TurnoutCmd(int id, int direction, int source) {
}

void INotify::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
}

void INotify::DCCFunc(int id, int bit, int newvalue, int source) {
}

void INotify::DCCFunc(int id, unsigned long int newvalue, int source) {
}

void INotify::setSettings(String key, String value) {
}

LinkedList<INotify::requestInfo*>* INotify::getRequestList() {
	return &requestList;
}



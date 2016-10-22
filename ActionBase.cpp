/*
 * ActionBase.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include "ActionBase.h"
#include <Arduino.h>

ActionBase::ActionBase() {
	// TODO Auto-generated constructor stub

}

ActionBase::~ActionBase() {
	// TODO Auto-generated destructor stub
}

String ActionBase::getHTMLCfg(String urlprefix) {
	return "";
}

String ActionBase::getHTMLController(String urlprefix) {
	return "";
}


void ActionBase::TurnoutCmd(int id, int direction, int source) {
}

void ActionBase::DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source) {
}

void ActionBase::DCCFunc(int id, int bit, int newvalue, int source) {
}

void ActionBase::DCCFunc(int id, unsigned long int newvalue, int source) {
}

void ActionBase::setSettings(String key, String value) {
}


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

void ActionBase::setSettings(int status) {
}

void ActionBase::loop() {
}

void ActionBase::TurnoutCmd(int id, int direction) {
}

void ActionBase::DCCSpeed(int id, int speed, int direction, int SpeedSteps) {
}

void ActionBase::DCCFunc(int id, int bit, int newvalue) {
}

void ActionBase::DCCFunc(int id, unsigned long int newvalue) {
}

/*
 * ActionReceiverMediator.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include <Arduino.h>
#include "FS.h"
#ifdef ESP32
	#include "SPIFFS.h"
#endif
#include "Controller.h"
#include "Utils.h"
#include "Logger.h"
#include "Consts.h"
#include "Webserver.h"


Controller::Controller() {
	cmdlogger = NULL;
	dccSniffer = NULL;
	longestLoop = 0;
	#ifdef ESP32
		if (!SPIFFS.begin(true)) {
	#else
		if (!SPIFFS.begin()) {
	#endif
		Logger::getInstance()->addToLog(LogLevel::ERROR, "SPIFFS konnte nicht genutzt werden!");
	}
	EMERGENCYActive = false;
//	for (int i = 0; i < 99; i++) {
//		l[i] = 0;
//	}
//	next = 0;
}

Controller::~Controller() {
}

void Controller::registerCmdReceiver(CmdReceiverBase* base) {
	receiver.add(base);
	loops.add(base);
}

void Controller::doLoops() {
	int idx;
	// Make sure that the nextRun List is as long as the loop list
	while (nextRun.size() < loops.size()) {
		nextRun.add(0);
	}
	// Run and save the new execute time
	unsigned long now = millis();
	for (idx = 0; idx < loops.size(); idx++) {
		if (nextRun.get(idx)  <= now) {
			int wait = loops.get(idx)->loop();
			nextRun.set(idx, millis() + wait);
		}
		delay(0);
	}
	logLoop(millis() - now);

	if (dnsServer) {
		dnsServer->processNextRequest();
	}
}

void Controller::registerNotify(INotify* base) {
	if (base == NULL) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Null in registeryNotify");
		return;
	}
	actions.add(base);
}

void Controller::registerLoop(ILoop* loop) {
	if (loop == NULL) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Null in registeryLoop");
		return;
	}
	loops.add(loop);
}

void Controller::notifyTurnout(int id, int direction, int source) {
	// TODO Wie bei den anderen Notify-Funktion Zustand speichern und nur tatsächliche Änderungen weiterleiten
	// ignore the same command within 50 msec
	if (lastTurnoutCmd[0] == id && lastTurnoutCmd[1] == direction
			&& (millis() - lastTurnoutCmd[2]) < 2000) {
		lastTurnoutCmd[2] = millis();
		return;
	}
	TurnOutData* data = getTurnOutData(id);
	data->direction = direction;
	Logger::log(LogLevel::TRACE, 
			"Turnout-CMD [ID:" + String(id) + "/ D:" + String(direction) + "]");
	lastTurnoutCmd[0] = id;
	lastTurnoutCmd[1] = direction;
	lastTurnoutCmd[2] = millis();

	// Send the information to the actions
	int idx;
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->TurnoutCmd(id, direction, source);
	}
}

void Controller::getHTMLController() {
	Webserver::sendContent("<div class=\"container\">");
	for (int idx = 0; idx < settings.size(); idx++) {
		String msg = settings.get(idx)->getHTMLController(
				"/set?id=" + String(idx) + "&");
		msg += "\n";
		Webserver::sendContent(msg);
	}
	Webserver::sendContent("</div>");
}

void Controller::getHTMLCfg() {
	Webserver::sendContent("<div class=\"container\">");
	for (int idx = 0; idx < settings.size(); idx++) {
		Webserver::sendContent(
			 settings.get(idx)->getHTMLCfg("/set?id=" + String(idx) + "&")
		);
		Webserver::sendContent("\n");
	}
	Webserver::sendContent("</div>");
}

void Controller::setRequest(String id, String key, String value) {
	int idx = id.toInt();
	if (idx >= settings.size()) {
		return;
	}
	settings.get(idx)->setSettings(key, value);
}

LocData* Controller::getLocData(int id) {
	LocData* data;
	if (items.find(id) == items.end()) {
		data = new LocData();
		data->status = 0;
		data->speed = 0;
		data->speedsteps = 0;
		if (id != Consts::LOCID_ALL) {
			items[id] = data;
		}
		return data;
	}
	return items[id];
}

TurnOutData* Controller::getTurnOutData(int id) {
	TurnOutData* data;
	if (turnoutinfo.find(id) == turnoutinfo.end()) {
		data = new TurnOutData();
		data->direction = 0;
		turnoutinfo[id] = data;
		return data;
	}
	return turnoutinfo[id];
}


void Controller::notifyGPIOChange(int pin, int newvalue) {
	Logger::log(LogLevel::TRACE, "Pin changed " + String(pin) + "/" + String(newvalue));
	for (int idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->GPIOChange(pin, newvalue);
	}
}

/**
 * @param speed see Consts.h
 * @param direction 1 = forward / -1 = reverse
 */
void Controller::notifyDCCSpeed(int id, int speed, int direction,
		int SpeedSteps, int source) {
	if (direction == 0) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Ungültige Richtung (0)");
	}
	EMERGENCYActive = speed == Consts::SPEED_EMERGENCY;
	// Filter out known commands
	LocData* data;
	if (items.find(id) == items.end()) {
		data = new LocData();
		if (id != Consts::LOCID_ALL) {
			items[id] = data;
		}
	} else {
		data = items[id];
		if (data->direction == direction && data->speed == speed
				&& data->speedsteps == SpeedSteps) {
			return;
		}
	}

	// Save new state
	data->direction = direction;
	data->speed = speed;
	data->speedsteps = SpeedSteps;
	// Logger::getInstance()->printf(Logger::DEBUG, 
	// 		("DCC-Speed: " + String(id) + " D: " + String(direction) + " "
	// 				+ String(speed) + " " + String(SpeedSteps)).c_str());
	Logger::getInstance()->printf(LogLevel::TRACE, "DCC-Speed: %d D: %d  %d %d", id, direction, speed, SpeedSteps);

	// Send the information to the actions
	int idx;
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->DCCSpeed(id, speed, direction, SpeedSteps, source);
	}
	if (id == Consts::LOCID_ALL) {
		delete data;
	}
}


void Controller::notifyDCCFun(int id, int bit, unsigned int newBitValue, int source) {
	// Get the old status ...
	LocData* data = getLocData(id);
	boolean changed = false;
	// .. and send only the changed bits
	unsigned long int value = data->status;
	unsigned long int oldBitValue = bit_is_set(value, bit);
	if (oldBitValue == newBitValue) {
		return;
	}
	changed = true;
	if (newBitValue == 0) {
		clear_bit(data->status, bit);
	} else {
		set_bit(data->status, bit);
	}
	for (int idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->DCCFunc(id, bit, (newBitValue == 0) ? 0 : 1, source);
	}
	if (changed) {
		Logger::log(LogLevel::TRACE, "Func " + String(id) + " " + String(data->status));
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, data->status, source);
		}
	}
}


void Controller::notifyDCCFun(int id, int startbit, int stopbit, unsigned long partValues, int source) {
	// Get the old status ...
	LocData* data = getLocData(id);

	boolean changed = false;
	// .. and send only the changed bits
	unsigned long int value = data->status;
	for (int i = startbit; i <= stopbit; i++) {
		unsigned long int oldBitValue = bit_is_set(value, i);
		unsigned long int newBitValue = bit_is_set(partValues, i);
		if (oldBitValue == newBitValue) {
			continue;
		}
		changed = true;
		if (newBitValue == 0) {
			clear_bit(data->status, i);
		} else {
			set_bit(data->status, i);
		}
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, i, (newBitValue == 0) ? 0 : 1, source);
		}
 	}
	if (changed) {
		Logger::log(LogLevel::TRACE, "Func " + String(id) + " " + String(data->status));
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, data->status, source);
		}
	}
}

String Controller::getHostname() {
	return "ly-dcc-" + Utils::getMAC();
}

void Controller::registerCmdSender(CmdSenderBase* base) {
	sender.add(base);
}

void Controller::updateRequestList() {
	requestList.clear();
	// Create Requestlist from all actions-Items
	for (int idx = 0; idx < actions.size(); idx++) {
		INotify* b = actions.get(idx);
		b->getRequestList(&requestList);
	}
	// Set Requestlist for all Senders
	for (int idx = 0; idx < sender.size(); idx++) {
		CmdSenderBase* s = sender.get(idx);
		s->setRequestList(&requestList);
	}

}

void Controller::emergencyStop(int source) {
	notifyDCCSpeed(Consts::LOCID_ALL, Consts::SPEED_EMERGENCY,
				   Consts::SPEED_FORWARD, 128, source);
}

void Controller::enableAPModus() {
	Logger::getInstance()->addToLog(LogLevel::INFO, "Aktiviere Access Point!");
 	WiFiMode_t mode = WiFi.getMode();
	if (mode == WIFI_AP || mode == WIFI_AP_STA) {
		Logger::getInstance()->addToLog(LogLevel::INFO, "Access Point bereits aktiv!");
		return;
	}
	int status = WiFi.status();
	if (status == WL_NO_SSID_AVAIL || status == WL_DISCONNECTED) {
		Logger::log(LogLevel::TRACE, "Station Modus abgeschalten");
		WiFi.disconnect();
	}
	WiFi.softAP("Hallo World");
	WiFi.enableAP(true);
	Logger::log(LogLevel::TRACE, "IP für AP: " + WiFi.softAPIP().toString());
	dnsServer.reset(new DNSServer());
	dnsServer->start(53, "*", WiFi.softAPIP());
}

bool Controller::isEmergency() {
	return EMERGENCYActive;
}

LinkedList<ISettings*>* Controller::getSettings() {
	return &settings;
}

void Controller::registerSettings(ISettings* loop) {
	if (loop == NULL) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Null in registerySettings");
		return;
	}
	settings.add(loop);
}

void Controller::logLoop(unsigned long now) {
//	if (now > 99) {
//		now = 99;
//	}
//	l[now]++;
//	if (next <= millis()) {
//		Serial.print(" 0: ");
//		for (int i = 0; i < 99; i++) {
//			Serial.printf(" %7d", l[i]);
//			if (i % 10 == 9) {
//				Serial.printf("\r\n%2d: ", (i + 1));
//			}
//		}
//		Serial.println();
//		next = millis() + 5000;
//	}
//	if (now > longestLoop) {
//		longestLoop = now;
//		Serial.println("Gesamt " + String(longestLoop));
//	}
}

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
#include "WebserverA.h"
#ifdef ESP8266
        #include <ESP8266WiFi.h>
#else
        #include <WiFi.h>
#endif
#include "InternalStatusWifiSys.h"
#include "SpeedKonverter.h"

Controller::Controller() {
	cmdlogger = NULL;
	dccSniffer = NULL;
	longestLoop = 0;
	#ifdef ESP32
		if (!SPIFFS.begin(true)) {
	#else
		if (!SPIFFS.begin()) {
	#endif
		Logger::log(LogLevel::ERROR, "CNT", "SPIFFS konnte nicht genutzt werden!");
	}
	EMERGENCYActive = false;
	for (int i = 0; i < 100; i++) {
		l[i] = 0;
	}
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
	unsigned long loopstarted = millis();
	for (idx = 0; idx < loops.size(); idx++) {
		if (nextRun.get(idx)  <= loopstarted) {
//			unsigned long started = millis();
			ILoop* loop = loops.get(idx);
			int wait = loop ->loop();
//			unsigned long stopped = millis();
			nextRun.set(idx, loopstarted + wait);
		}
		delay(0);
	}
	if (next < loopstarted) {
//		Logger::log(LogLevel::INFO, "Running");
		next = loopstarted + 1000;
	}
	logLoop(millis() - loopstarted);
	if (dnsServer) {
		dnsServer->processNextRequest();
	}

}

void Controller::registerNotify(INotify* base) {
	if (base == NULL) {
		Logger::log(LogLevel::ERROR, "CNT", "Null in registerNotify");
		return;
	}
	actions.add(base);
}

void Controller::registerConnectors(Connectors* base) {
	if (base == NULL) {
		Logger::log(LogLevel::ERROR, "CNT", "Null in registerConnectors");
		return;
	}
	connectors.add(base);
}

void Controller::registerLoop(ILoop* loop) {
	if (loop == NULL) {
		Logger::log(LogLevel::ERROR, "CNT", "Null in registeryLoop");
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
	Logger::log(LogLevel::TRACE, "CNT", 
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
		sendContent(settings.get(idx)->getHTMLController("/set?id=" + String(idx) + "&"));
	}
	Webserver::sendContent("</div>");
}

void Controller::getHTMLCfg() {
	sendContent("<div class=\"container\">");
	for (int idx = 0; idx < settings.size(); idx++) {
		settings.get(idx)->getHTMLConfig("/set?id=" + String(idx) + "&", this);
		sendContent("\n");
	}
	sendContent("</div>");
}

void Controller::setRequest(String id, String key, String value) {
	if (id == "sys") {
		Serial.println("Active");
		InternalStatusWifiSys::handleRequest(key, value);
		return;
	}
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
		data->speed = Consts::SPEED_STOP;
		data->speedsteps = Consts::DEFAULTSPEEDSTEPS;
		data->direction = Consts::SPEED_FORWARD;
		if (id != Consts::LOCID_ALL) {
			items[id] = data;
			itemskeys.push_back(id);
		}
		return data;
	}
	return items[id];
}

String Controller::createDebugDiagramm() {
	String out = "";
	for (int i = 0; i < connectors.size(); i++) {
		Connectors* a = connectors.get(i);
		out = out + a->createDebugDiagramm("");
	}
	return out;
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
	//Logger::log(LogLevel::TRACE, "Pin changed " + String(pin) + "/" + String(newvalue));
	for (int idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->GPIOChange(pin, newvalue);
	}
}

void Controller::notifySpeeSteps(int id, int SpeedSteps) {
	if (id == Consts::LOCID_ALL) {
		return;
	}
	// Filter out known commands
	LocData* data;
	if (items.find(id) == items.end()) {
		data = new LocData();
		data->status = 0;
		data->speed = 0;
		data->speedsteps = SpeedSteps;
	} else {
		data = items[id];
		data->speedsteps = SpeedSteps;
	}
}

/**
 * @param speed see Consts.h
 * @param direction 1 = forward / -1 = reverse
 */
LocData* Controller::notifyDCCSpeed(int id, int speed, int direction, int source) {
	if (direction == 0) {
		Logger::log(LogLevel::ERROR, "CNT", "DCC-Speed: Ungültige Richtung (0)");
	}
	EMERGENCYActive = speed == Consts::SPEED_EMERGENCY;
	// Filter out known commands
	LocData* data;
	if (items.find(id) == items.end()) {
		data = new LocData();
		data->speedsteps = Consts::DEFAULTSPEEDSTEPS;
		data->status = 0;
		if (id != Consts::LOCID_ALL) {
			items[id] = data;
			itemskeys.push_back(id);
		}
	} else {
		data = items[id];
		bool sameSpeed = (SpeedKonverter::fromInternal(data->speedsteps, data->speed) ==  SpeedKonverter::fromInternal(data->speedsteps, speed));
		if (sameSpeed && source == Consts::SOURCE_RCKP) {
			// Beim Kontroller genauer prüfen
			sameSpeed = (data->speed == speed);
		}
//		Serial.println("SameSpeed: " + String(sameSpeed) + " " + String(data->speed) + ":" + String(speed) + " Source: " + String(source));
		if (data->direction == direction && sameSpeed ) {
			return nullptr;
		}
	}

	data->direction = direction;
	data->speed = speed;
	Logger::getInstance()->printf(LogLevel::TRACE, "[CNT] DCC-Speed: ID: %d S: %d D: %d", id, speed, direction);

	// Send the information to the actions
	int idx;
	for (idx = 0; idx < actions.size(); idx++) {
		actions.get(idx)->DCCSpeed(id, speed, direction, source);
	}
	if (id == Consts::LOCID_ALL) {
		delete data;
		return nullptr;
	}
	return data;
}


LocData* Controller::notifyDCCFun(int id, int bit, unsigned int newBitValue, int source) {
	// Get the old status ...
	LocData* data = getLocData(id);
	boolean changed = false;
	// .. and send only the changed bits
	unsigned long int value = data->status;
	unsigned long int oldBitValue = bit_is_set01(value, bit);
	if (oldBitValue == newBitValue) {
		return nullptr;
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
		Logger::log(LogLevel::TRACE, "CNT", "Func changed: ID:" + String(id) + " V:" + String(data->status));
		for (int idx = 0; idx < actions.size(); idx++) {
			actions.get(idx)->DCCFunc(id, data->status, source);
		}
	}
	return data;
}


void Controller::notifyDCCFun(int id, int startbit, int stopbit, unsigned long partValues, int source) {
	// Get the old status ...
	LocData* data = getLocData(id);

	boolean changed = false;
	// .. and send only the changed bits
	unsigned long int value = data->status;
	for (int i = startbit; i <= stopbit; i++) {
		unsigned long int oldBitValue = bit_is_set01(value, i);
		unsigned long int newBitValue = bit_is_set01(partValues, i);
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
		Logger::log(LogLevel::TRACE, "CNT", "Func changed: ID:" + String(id) + " V:" + String(data->status));
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

/**
 * Sammelt von Modulen die Information ein, welche Lok-ID und Weichen-ID benötigt werden
 *
 * und
 *
 * übermittelt diese Information an alle Befehlssender
 */
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

void Controller::emergencyStop(int source, bool enable) {
	// Ignore multiple emergencyStop-Commands
	if (enable == EMERGENCYActive) {
		return;
	}
	if (enable) {
		notifyDCCSpeed(Consts::LOCID_ALL, Consts::SPEED_EMERGENCY,
					Consts::SPEED_FORWARD, source);
	}
	EMERGENCYActive = enable;
	LinkedList<CmdSenderBase*>* list = getSender();
	for (int i = 0; i < list->size(); i++) {
		CmdSenderBase* b = list->get(i);
		if (b == NULL) {
			Logger::log(LogLevel::ERROR, "Controller: Sender is null");
			continue;
		}
		b->notifyEmergencyStop(source, enable);
	}
}

void Controller::enableAPModus() {
	Logger::log(LogLevel::INFO, "CNT", "Aktiviere Access Point!");
 	WiFiMode_t mode = WiFi.getMode();
	if (mode == WIFI_AP || mode == WIFI_AP_STA) {
		Logger::log(LogLevel::INFO, "CNT", "Access Point bereits aktiv!");
		return;
	}
	int status = WiFi.status();
	if (status == WL_NO_SSID_AVAIL || status == WL_DISCONNECTED) {
		Logger::log(LogLevel::TRACE, "CNT", "Station Modus wird deaktiviert");
		WiFi.disconnect();
	}
	WiFi.softAP("Hallo World");
	WiFi.enableAP(true);
	Logger::log(LogLevel::TRACE, "CNT", "IP für AP: " + WiFi.softAPIP().toString());
	dnsServer.reset(new DNSServer());
	dnsServer->start(53, "*", WiFi.softAPIP());
}

bool Controller::isEmergency() {
	return EMERGENCYActive;
}

LinkedList<ISettings*>* Controller::getSettings() {
	return &settings;
}

void Controller::sendPipeFilter(String dest, String key, String value) {
	for (int idx = 0; idx < getSettings()->size(); idx++) {
		ISettings* s = getSettings()->get(idx);
		if (s->getName().equals(dest)) {
			Serial.println("Sending " + key + " with  " + value + " to " + dest);
			s->setSettings(key, value);
		}
	}
}

void Controller::registerSettings(ISettings* loop) {
	if (loop == NULL) {
		Logger::log(LogLevel::ERROR, "Null in registerySettings");
		return;
	}
	Serial.println("Adding Settings " + loop->getName());
	settings.add(loop);
	status.add(loop);
}

void Controller::registerStatus(IStatus* loop) {
	if (loop == NULL) {
		Logger::log(LogLevel::ERROR, "Null in registeryStatus");
		return;
	}
	status.add(loop);
}

void Controller::sendContent(String s) {
		Webserver::sendContent(s);
}

void Controller::logLoop(unsigned long now) {
	if (now > 99) {
		now = 99;
	}
	l[now]++;
}


String Controller::getInternalStatus(String modul, String key) {
	statusAsString.reset();
	collectAllInternalStatus(&statusAsString, modul, key);
	return statusAsString.get();
}

void Controller::collectAllInternalStatus(IInternalStatusCallback* cb, String modul, String key) {
	cb->reset();
	for (int i = 0; i < status.size(); i++) {
		IStatus* s = status.get(i);
		if (modul.equals("*") || s->getName().equals(modul)) {
			s->getInternalStatus(cb, key);
		}
		if (modul.equals("*") || modul.equals("moduls")) {
			cb->send("modules", String(i), s->getName());
		}
	}
	if (modul.equals("*") || modul.equals("moduls")) {
		int idx = status.size();
		cb->send("modules", String(idx++), "sys");
 		cb->send("modules", String(idx++), "wifi");
		cb->send("modules", String(idx++), "loc");
		cb->send("modules", String(idx++), "turnout");
		cb->send("modules", String(idx++), "log");
	}
 	InternalStatusWifiSys::getInternalStatus(cb, modul, key);
	internalStatusObjStatus(cb, modul, key);
}


String Controller::getInternalStatusAsJon(String modul, String key) {
	collectAllInternalStatus(&statusAsJson, modul, key);
	String output = statusAsJson.get();
	statusAsJson.reset();
	return output;
}

void Controller::printInternalStatusAsJon() {
	collectAllInternalStatus(&statusAsJson, "*", "*");
	statusAsJson.print();
}


void Controller::internalStatusObjStatus(IInternalStatusCallback* cb, String modul, String key) {
	if (modul.equals("loc") || modul.equals("*")) {
		for (std::map<int,LocData*>::iterator it = items.begin(); it != items.end(); ++it) {
				if (key.equals("*") || String(it->first).equals(key)) {
		 			cb->send("loc", String(it->first), String(it->second->direction) + " / " +
		 		 	String(it->second->speed) + " / " +
		 		 	String(it->second->speedsteps) + " / " +
		 		 	String(it->second->status));
				}
		}
	}
	if (modul.equals("turnout") || modul.equals("*")) {
		for (std::map<int,TurnOutData*>::iterator it = turnoutinfo.begin(); it != turnoutinfo.end(); ++it) {
				if (key.equals("*") || String(it->first).equals(key)) {
		 			cb->send("turnout", String(it->first), String(it->second->direction));
				}
		}
	}
	if (modul.equals("timing") || modul.equals("*")) {
		for (int idx = 0; idx < 100; idx++) {
			if (l[idx] > 0) {
		 		cb->send("timing", String(idx) , String(l[idx]));
			}
		}
	}

    if (key == "requestlist" || key == "*") {
        LinkedList<INotify::requestInfo*>* l = getRrequestList();
		if (l != NULL) {
        	for (int i =0; i < l->size(); i++) {
            	INotify::requestInfo* d = l->get(i);
				cb->send("requestlist", (d->art == d->LOCO) ? "Lok" : "Weiche", String(d->id));
        	}
		}
	}

	if (modul.equals("cnt") || modul.equals("*")) {
		if (key.equals("*") || key.equals("estop")) {
				cb->send("cnt", "estop", String(isEmergency()));
		}
	}

}

Controller::Items* Controller::getLocData() {
	return &items;
}

std::vector<int>* Controller::getLocDataKeys() {
	return &itemskeys;
}

LinkedList<INotify::requestInfo*>* Controller::getRrequestList() {
	return &requestList;
}

/**
 * Informiert andere(!) Geräte  über eine durch dieses Gerät gewünschte Änderung an einer Weiche
 */
void Controller::sendSetTurnout(String id, String status) {
	LinkedList<CmdSenderBase*>* list = getSender();
    for (int i = 0; i < list->size(); i++) {
    	CmdSenderBase* b = list->get(i);
    	if (b == NULL) {
			Logger::log(LogLevel::ERROR, "ActionSendTurnoutCommand: Sender is null");
    		continue;
    	}
    	b->sendSetTurnout(String(id), status);
    }

}

/**
 * Informiert andere(!) Geräte  über eine durch dieses Gerät gewünschte Änderung an einem Sensor
 */
void Controller::sendSetSensor(uint16_t id, uint8_t status) {
  LinkedList<CmdSenderBase*>* list = getSender();
  for (int i = 0; i < list->size(); i++) {
    CmdSenderBase* b = list->get(i);
    if (b == NULL) {
      Logger::log(LogLevel::ERROR, "ActionSendSensorCommand: Sender is null");
      continue;
    }
    b->sendSetSensor(id, status);
  }
}


/**
 * Informiert andere(!) Geräte  über eine durch dieses Gerät gewünschte Änderung an einer Lok Geschwindigkeit
 */
void Controller::sendDCCSpeed(int id, int speed, int direction, int source) {
  LocData* d = notifyDCCSpeed(id, speed, direction, Consts::SOURCE_RCKP);
  if (d == nullptr) {
		return;
  }
  LinkedList<CmdSenderBase*>* list = getSender();
  for (int i = 0; i < list->size(); i++) {
	CmdSenderBase* b = list->get(i);
    if (b == NULL) {
      Logger::log(LogLevel::ERROR, "ActionSendSensorCommand: Sender is null");
      continue;
    }
    b->sendDCCSpeed(id, d);
  }
}

void Controller::sendDCCFun(int id, int bit, unsigned int value, int source) {
  LocData* d = notifyDCCFun(id, bit, value, source);
  if (d == nullptr) {
		return;
  }
  LinkedList<CmdSenderBase*>* list = getSender();
	Serial.println("Found Sender " + String(list->size()));
  for (int i = 0; i < list->size(); i++) {
	CmdSenderBase* b = list->get(i);
    if (b == NULL) {
      Logger::log(LogLevel::ERROR, "ActionSendSensorCommand: Sender is null");
      continue;
    }
    b->sendDCCFun(id, d, bit);
  }
}

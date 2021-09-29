/*
 * CmdReceiverESPNOW.cpp
 *
 *  Created on: 01.01.2018
 *      Author: sven
 * 
 * 
 */

#include <algorithm>
#include "CmdReceiverESPNOW.h"
#include "Logger.h"
#include "Utils.h"
#include "WifiCheck.h"

void CmdReceiverESPNOW::setKey(String masterkey) {
	for (int i  = 0; i < 16; i++) {
		key[i] = Utils::hextoint(masterkey[i * 2]) * 16 + Utils::hextoint(masterkey[i * 2 + 1]);
	}	
	#ifdef espnowEncrypt 
		cipher.setKey(key, cipher.keySize());
	#endif
}

CmdReceiverESPNOW::CmdReceiverESPNOW(Controller* c, String rollenName, String masterkey, int kanal) : Z21Format(c) {
	setModulName("ESPNOW");

	status = "Init";
	instance = this;
	espnowChannel = kanal;
	myid = getID();
	msgsize = sizeof(msg);
	Serial.printf("Myid: %08X\r\n", myid);
	Serial.println("Starting ESP NOW");

	if (!setRolle(rollenName)) {
		return;
	}
	setKey(masterkey);
	checkAP(kanal);
	initESPNOW(kanal);

	if (rolle == unknown) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "ESPNOW: Unbekannte Rolle " + rolle);
		return;
	}
	if (sizeof(msg) != 32) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "ESPNOW: Nachrichtengröße nicht 32 bytes");
		return;
	}
	init = true;
	if (rolle == master) {
		status = "No Clients";
	} else {
		status = "No Master";
	}
}

bool CmdReceiverESPNOW::setRolle(String rollenName) {
	if (rollenName.equals("master")) {
		rolle = master;
	} else if (rollenName.equals("receiver")) {
		rolle = receiver;
	} else if (rollenName.equals("repeater")) {
		rolle = repeater;
	} else if (rollenName.equals("receiver+repeater")) {
		rolle = receiverrepeater;
	} else {
		status = "Role unknown";
		rolle = unknown;
		return false;
	}
	rolename = rollenName;
	return true;
}

CmdReceiverESPNOW::~CmdReceiverESPNOW() {
}


int CmdReceiverESPNOW::loop() {
	if (rolle == unknown || !init) {
		return 1000;
	}
	// Handle Msg from the Query
	if (receivedQueue.size() > 0) {
		handleReceived();
	}
	// Send Messages from Query
	if (sendQueue.size() > 0) {
		msg* m = sendQueue.pop();
		addMsgId(m->id);
		#ifdef espnowEncrypt 
			unsigned char buffer[32];
	   		memset(buffer, 0, 32);
	    	cipher.encryptBlock(buffer, reinterpret_cast<unsigned char*>(m));		
	    	cipher.encryptBlock(buffer + 16, reinterpret_cast<unsigned char*>(m) + 16);		
			esp_now_send(broadcastMac, buffer, msgsize);
		#else
			esp_now_send(broadcastMac, reinterpret_cast<unsigned char*>(m), msgsize);
		#endif
		free(m);
	}
	// Send a Echo Request every 5 sec, if I'm the master
	if (rolle == master &&  Utils::timeDiff(last, 5000)) {
		Serial.println("=============================================" + String(millis()));
		status = "D: " + String(master_clientsFound);
		master_clientsFound = 0;
		sendEchoRequest();
		last = millis();
		lastMasterReceived = last; 			
	}
	// Enable Broadcast on a regular base or request Information about Loco or Turnout
	if (isReceiver() &&  Utils::timeDiff(last, 1000)) {
		if (requestList == nullptr) {
			enableBroadcasts();
		} else {
			if (requestList->size() > 0) {
				INotify::requestInfo* ri = requestList->get(loopStatus);
				if (ri->art == INotify::requestInfo::LOCO) {
					requestLocoInfo(ri->id);
				} else if (ri->art == INotify::requestInfo::TURNOUT) {
					requestTurnoutInfo(ri->id);
				}
				loopStatus = (loopStatus + 1) % requestList->size();
			}
		}
		last = millis();
	}
	if (rolle == master &&  loklist.size() > 0 && Utils::timeDiff(last, 1000 / loklist.size()))  {
		int id = loklist.at(loopStatus);
		// TODO
		loopStatus = (loopStatus + 1) % loklist.size();
		last = millis();
	}
	if ((lastMasterReceived > 0) && Utils::timeDiff(lastMasterReceived, 2000) && rolle != master) {
		Logger::getInstance()->addToLog(LogLevel::WARNING, "ESP-Now Master Timeout");
		// TODO cnt->emergencyStop(Consts::SOURCE_WLAN, true);
		status = "Master lost";
		lastMasterReceived = 0;
	}
	if (Utils::timeDiff(lastCheck, 5000)) {
		int32_t sta = WifiCheck::getWifiChannel();
		int32_t ap = WifiCheck::getAPChannel();
		boolean staDown = false;
		if (sta != ap) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Channel AP ST unterschiedlich " + String(ap) + "/" + String(sta));
			staDown = true;
		}
		if (ap != espnowChannel) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Channel für AP für ESP-NOW falsch SOLL:" + String(espnowChannel) + "  Ist: " + String(ap));
			staDown = true;
		}
		if (staDown) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Deaktiviere Station-Modus");
			WiFi.mode(WIFI_AP); // Must be AP!
			WiFi.softAP(("sender" + String(myid)).c_str(), "sendersender", espnowChannel);
		}
		lastCheck = millis();
	}

	return 5;
}

/***
 * Receive the Message, decrypt it and put it to the receivedQuery
 */
#ifdef ESP32
void CmdReceiverESPNOW::cb_MsgReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
#endif 
#ifdef ESP8266
void CmdReceiverESPNOW::cb_MsgReceived(u8 *mac_addr, u8 *data, u8 len) {
#endif
	if (len != msgsize) {
		Serial.println("Rejecting ESPNOW Message: " + String(msgsize) + "/" + String(len));
		return;
	}
	#ifdef espnowEncrypt
		msg* m = new msg;
		CmdReceiverESPNOW::instance->cipher.decryptBlock(reinterpret_cast<unsigned char*>(m), data);
		CmdReceiverESPNOW::instance->cipher.decryptBlock(reinterpret_cast<unsigned char*>(m) + 16, data + 16);
	#else
		memcpy(m, recevied,len);
	#endif
	instance->receivedQueue.add(m);
}

/**
 * Create a Echo Request Message
 */
void CmdReceiverESPNOW::sendEchoRequest() {
		msg* m = new msg;
	   	memset(m, 0, sizeof(msg));
		m->hops = 0;
		m->id = Utils::getRnd();;
		m->typ = messagetyps::internal;
		m->subtyp = internalSubs::echoRequest;
		sendQueue.add(m);
}

/**
 * Will be called from the z21 submodul
 * encapsulate z21 command into a espnow message
 */
void CmdReceiverESPNOW::send() {
	msg* m = new msg;
   	memset(m, 0, sizeof(msg));
	m->hops = 0;
	m->id = Utils::getRnd();
	if (rolle == master) {
		m->typ = messagetyps::z21FromMaster;
	} else {
		m->typ = messagetyps::z21ToMaster;
	}
	for (int i = 0; i < pb[0]; i++) {
		m->msg[i] = pb[i];
	}
	messageCmdSend++;
	sendQueue.add(m);
}

/**
 * Add Message ID to Ring Buffer
 */
void CmdReceiverESPNOW::addMsgId(uint32_t id) {
	msgIds[msgIdsIdx] = id;
	msgIdsIdx = (msgIdsIdx + 1) % msgIdsSize;
	messageCount++;
}

/**
 * Testet, ob die Nachricht bereit empfangen wurde
 */
bool CmdReceiverESPNOW::existsMsgId(uint32_t id) {
	for (int i = 0; i < msgIdsSize; i++) {
	 	if (msgIds[i] == id) {
	 		return true;
		}
	}
	messageCount++;
	return false;
}

void CmdReceiverESPNOW::sendEchoReply() {
	msg* m = new msg;
   	memset(m, 0, sizeof(msg));
	m->hops = 0;
	m->id = Utils::getRnd();
	m->typ = messagetyps::internal;
	m->subtyp = internalSubs::echoReply;
	m->msg[0] = myid & 0xFF;
	m->msg[1] = (myid >> 8) & 0xFF;
	m->msg[2] = (myid >> 16) & 0xFF;
	m->msg[3] = (myid >> 24) & 0xFF;
	sendQueue.add(m);
}

void CmdReceiverESPNOW::handleEchoReply(msg* m) {
	if (rolle != master) {
		return;
	}
	String hop = String(myid, HEX);
	Serial.printf("Receive Reply from %02X%02X%02X%02X Hops: %d (%lu)\r\n", m->msg[0], m->msg[1], m->msg[2], m->msg[3], m->hops, millis());
	Serial.println("I'm " + hop);
	master_clientsFound++;
	// for (int idx = m->hops ; idx >= 0; idx--) {
	// 	String id = String(m->msg[idx * 4], HEX) + String(m->msg[idx * 4 + 1], HEX) + String(m->msg[idx * 4 + 2], HEX) + String(m->msg[idx * 4 + 3], HEX);
	// 	status += "\"" + hop + "\" -> \"" + id + "\";";
	// 	hop = id;
	// }
}

void CmdReceiverESPNOW::getInternalStatus(IInternalStatusCallback* cb, String key) {
	String name = getName();
	if (key.equals("status") || key.equals("*")) {
		cb->send(name, "status", status);
	}
	if (key.equals("role") || key.equals("*")) {
		cb->send(name, "role", rolename);
	}
	if (key.equals("msgcount") || key.equals("*")) {
		cb->send(name, "msgcount", String(messageCount));
	}
	if (rolle != master && (key.equals("cmdreceived") || key.equals("*"))) {
		cb->send(name, "cmdreceived", String(messageCmdReceived));
	}
	if (rolle == master && (key.equals("cmdsend") || key.equals("*"))) {
		cb->send(name, "cmdsend", String(messageCmdSend));
	}
	if (key.equals("id") || key.equals("*")) {
		cb->send(name, "id", String(myid, HEX));
	}
}

void CmdReceiverESPNOW::sendSetTurnout(String id, String status) {
  // TODO
}
void CmdReceiverESPNOW::sendSetSensor(uint16_t id, uint8_t status) {
  // TODO
}

void CmdReceiverESPNOW::sendDCCSpeed(int id, LocData* d) {
	createDCCSpeedCmd(id, d);
	send();
}
void CmdReceiverESPNOW::sendDCCFun(int id, LocData* d,  unsigned int changedBit) {
	sendDCCSpeed(id, d);
}


String CmdReceiverESPNOW::getName() {
	return "espnow";
}  

/**
 * Wird vom z21-Parser aufgerufen, falls die Information für eine Lok-ID angefordert wird 
 */
void CmdReceiverESPNOW::adjustBroadcast(int addr) {
	if ( std::find(loklist.begin(), loklist.end(), addr) == loklist.end() ) {
		loklist.push_back(addr);
	}
}

uint32_t CmdReceiverESPNOW::getID() {
	#ifdef ESP32
	uint64_t chipid=ESP.getEfuseMac();
	return ((uint32_t)chipid ^ (uint32_t)(chipid >> 16));
	#endif
	#ifdef ESP8266
	return system_get_chip_id();
	#endif
}

void CmdReceiverESPNOW::checkAP(int kanal) {
	bool apNeeded =true;
	if (WifiCheck::wifiISAPActive()) {
		if (WifiCheck::getAPChannel() == kanal) {
			Logger::log(LogLevel::INFO, "[ESPNOW] Bestender AP wird genutzt.");
			apNeeded = false;
		} else {
			Logger::log(LogLevel::ERROR, "[ESPNOW] Bestende AP kann nicht genutzt. (keine Kanalübereinstimmung)");
		}
	} 
	if (apNeeded) {
		bool b = WiFi.softAP(("sender" + String(myid)).c_str(), "sendersender", kanal);
		if (!b) {
			Logger::log(LogLevel::ERROR, "[ESPNOW] AP konnte nicht gestartet werden!");
			return;
		}
		Logger::log(LogLevel::INFO, "[ESPNOW] Neuer AP gestartet. " + String(b));
	}
	WiFi.mode(WIFI_AP_STA ); // Must be AP_STA!
}
void CmdReceiverESPNOW::initESPNOW(int kanal) {
	int ret = esp_now_init();
	if (ret == 0) {
		//
	} else {
		Logger::log(LogLevel::ERROR, "ESP_NOW: FAILED! ERR: " + String(ret));
		return;
	}
	delay(10);

	#ifdef ESP32
 		esp_now_peer_info_t slave;
   		memset(&slave, 0, sizeof(slave));
   		for (int i = 0; i < 6; ++i)
 	    	slave.peer_addr[i] = 0xFF;
   		slave.channel = kanal; // pick a channel
   		slave.encrypt = 0; // no encryption
		const esp_now_peer_info_t *peer = &slave;
		esp_now_add_peer(peer);
	#endif
	#ifdef ESP8266
		esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
	#endif
	esp_now_register_recv_cb(CmdReceiverESPNOW::cb_MsgReceived);
}

void CmdReceiverESPNOW::handleReceived() {
		msg* m = receivedQueue.pop();
		if (existsMsgId(m->id)) {
			// Already known, Ignore Message
			free(m); 
			return;
		} 
		addMsgId(m->id);
		if (isRepeater()) {
			m->hops++;
			if (m->typ == messagetyps::internal 
				&& m->subtyp == internalSubs::echoReply
				&& m->hops < 5) {
				int idx = m->hops * 4;
				m->msg[idx] = myid & 0xFF;
				m->msg[idx + 1] = (myid >> 8) & 0xFF;
				m->msg[idx + 2] = (myid >> 16) & 0xFF;
				m->msg[idx + 3] = (myid >> 24) & 0xFF;
			}
			sendQueue.add(m);
		}
		if (m->typ == messagetyps::z21FromMaster) {
			lastMasterReceived = last; 	
			status = "Connect";
			messageCmdReceived++;		
			if (rolle != master) {
				bool p = parseServer2Client(m->msg, m->msg[0]);
				if (!p) {
					printPacketBuffer("Cannot parse (from Master): ", m->msg, m->msg[0]);
				}
			}
		} else if (m->typ == messagetyps::z21ToMaster) {
			if (rolle == master) {
				bool p = parseClient2Server(m->msg, m->msg[0]);
				if (!p) {
					printPacketBuffer("Cannot parse (to Master): ", m->msg, m->msg[0]);
				}
			}
		} else 	if (m->typ == messagetyps::internal) {
			if (m->subtyp == internalSubs::echoRequest) {
				sendEchoReply();
			} else if (m->subtyp == internalSubs::echoReply) {
				handleEchoReply(m);
			} else {
				Serial.println("Unbekannter Sub Message-Typ");
			}
		} else {
			Serial.println("Unbekannter Message-Typ");
			for (int i = 0; i < msgsize; i++) {
				Serial.print(String(i, HEX));
			}
			Serial.println();
		}
		// Free message if not longer used
		if (!isRepeater()) {
			free(m);
		}
}

bool CmdReceiverESPNOW::isReceiver() {
	return rolle == receiver || rolle == receiverrepeater;
}

bool CmdReceiverESPNOW::isRepeater() {
	return rolle == repeater || rolle == receiverrepeater;
}

int  CmdReceiverESPNOW::msgsize = sizeof(CmdReceiverESPNOW::msg);
CmdReceiverESPNOW* CmdReceiverESPNOW::instance = NULL;

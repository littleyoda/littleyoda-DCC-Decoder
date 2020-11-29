/*
 * CmdReceiverESPNOW.cpp
 *
 *  Created on: 01.01.2018
 *      Author: sven
 * 
 * 
 */

#include "CmdReceiverESPNOW.h"
#include "Logger.h"
#include "Utils.h"

CmdReceiverESPNOW::CmdReceiverESPNOW(Controller* c, String rollenName, String masterkey, int kanal) : Z21Format(c) {
	setModulName("ESPNOW");
	oldstatus = "";
	status = "";
	setName("ESPNOW");
	setModulName("ESPNOW");
	#ifdef ESP32
	uint64_t chipid=ESP.getEfuseMac();
	myid = ((uint32_t)chipid ^ (uint32_t)(chipid >> 16));
	#endif
	#ifdef ESP8266
	myid = system_get_chip_id();
	#endif
	msgsize = sizeof(msg);
	if (sizeof(msg) != 32) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "ESPNOW: Nachrichtengröße nicht 32 bytes");
		return;
	}
	Serial.printf("Myid: %08X\r\n", myid);
	instance = this;
	Serial.println("Starting ESP NOW " + String(sizeof(msg)));
	for (int i  = 0; i < 16; i++) {
		key[i] = Utils::hextoint(masterkey[i * 2]) * 16 + Utils::hextoint(masterkey[i * 2 + 1]);
	}	
	#ifdef espnowEncrypt 
		cipher.setKey(key, cipher.keySize());
	#endif
	rolle = unknown;
	if (rollenName.equals("master")) {
		rolle = master;
	} else if (rollenName.equals("repeater")) {
		rolle = repeater;
	} else if (rollenName.equals("receiver")) {
		rolle = receiver;
	} else {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "ESPNOW: Unbekannte Rolle " + rolle);
		return;
	}
	rolename = rollenName;

	bool b = WiFi.softAP(("sender" + String(myid)).c_str(), "sendersender", kanal);
	Serial.println("ESPNOW: SoftAP " + String(b) + " " + rollenName);
	WiFi.mode(WIFI_AP_STA ); // Must be AP_STA!

	int ret = esp_now_init();
	if (ret == 0) {
		Logger::log(LogLevel::ERROR, "ESP_NOW: OK");
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

CmdReceiverESPNOW::~CmdReceiverESPNOW() {
}


int CmdReceiverESPNOW::loop() {
	if (rolle == unknown) {
		return 1000;
	}
	// Handle Msg from the Query
	if (receivedQuery.size() > 0) {
		msg* m = receivedQuery.pop();
		if (existsMsgId(m->id)) {
			// Already known, Ignore Message
			free(m); 
		} else {
			addMsgId(m->id);
			if (rolle == repeater) {
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
				sendQuery.add(m);
			}
			if (m->typ == messagetyps::z21FromMaster) {
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
			if (rolle != repeater) {
				free(m);
			}
		}
	}
	if (sendQuery.size() > 0) {
		msg* m = sendQuery.pop();
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
	// Send a Echo Request every 1 sec, if I'm the master
	if (rolle == master &&  (last + 10000) < millis()) {
		Serial.println("=============================================" + String(millis()));
		sendEchoRequest();
		last = millis();
	}
	// Enable Broadcast on a regular base or request Information about Loco or Turnout
	if (rolle == receiver &&  (last + 5000) < millis()) {
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
	instance->receivedQuery.add(m);
}

/**
 * Create a Echo Request Message
 */
void CmdReceiverESPNOW::sendEchoRequest() {
		oldstatus = status;
		status = "";
		msg* m = new msg;
	   	memset(m, 0, sizeof(msg));
		m->hops = 0;
		m->id = Utils::getRnd();;
		m->typ = messagetyps::internal;
		m->subtyp = internalSubs::echoRequest;
		sendQuery.add(m);
}

/**
 * Will be called from the z21 submodul
 */
void CmdReceiverESPNOW::send() {
	if (pb[0] > 22) {
		Logger::log(LogLevel::ERROR, "Oversized Message detected");
		return;
	}
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
	sendQuery.add(m);
}

/**
 * Add Message ID to Ring Buffer
 */
void CmdReceiverESPNOW::addMsgId(uint32_t id) {
	msgIds[msgIdsIdx] = id;
	msgIdsIdx = (msgIdsIdx + 1) % msgIdsSize;
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
	sendQuery.add(m);
}

void CmdReceiverESPNOW::handleEchoReply(msg* m) {
	if (rolle != master) {
		return;
	}
	String hop = String(myid, HEX);
	Serial.printf("Receive Reply from %02X%02X%02X%02X Hops: %d (%lu)\r\n", m->msg[0], m->msg[1], m->msg[2], m->msg[3], m->hops, millis());
	Serial.println("I'm " + hop);
	for (int idx = m->hops ; idx >= 0; idx--) {
		String id = String(m->msg[idx * 4], HEX) + String(m->msg[idx * 4 + 1], HEX) + String(m->msg[idx * 4 + 2], HEX) + String(m->msg[idx * 4 + 3], HEX);
		status += "\"" + hop + "\" -> \"" + id + "\";";
		hop = id;
		setModulName("DCC Decoder");
	}
}

void CmdReceiverESPNOW::getInternalStatus(IInternalStatusCallback* cb, String key) {
	if (key.equals("status") || key.equals("*")) {
		cb->send("espnow", "status", status);
	}
}


String CmdReceiverESPNOW::createDebugDiagramm(String parent) {
	return getName() + "[label =\" " + getModulName() + "\\n" + getConfigDescription() + "\"];\r\n"
		      + parent + " -- " + getName() + ";\r\n";

}

int  CmdReceiverESPNOW::msgsize = sizeof(CmdReceiverESPNOW::msg);
CmdReceiverESPNOW* CmdReceiverESPNOW::instance = NULL;




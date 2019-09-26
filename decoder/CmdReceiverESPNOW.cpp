/*
 * CmdReceiverESPNOW.cpp
 *
 *  Created on: 01.01.2018
 *      Author: sven
 * 
 * TODO
 * - Auf eine funktionierende Broadcast Funktion für den ESP8266 warten
 * - Verschlüsselung, um Abschottung gegen andere ESPNOW Netze zu ermöglichen
 * 
 */

#ifdef ESP32
#include "CmdReceiverESPNOW.h"
#include "mbedtls/aes.h"

//

CmdReceiverESPNOW::CmdReceiverESPNOW(Controller* c, String rollenName) : z21PaketParser(c) {
	instance = this;
	Serial.println("Starting ESP NOW " + String(sizeof(msg)));
	msgsize = sizeof(msg);
	rolle = unknown;
	// if (sizeof(u8) != sizeof(uint8)) {
	// 	Logger::getInstance()->addToLog(LogLevel::ERROR, "ESPNOW: Fehler in der Datenstruktur");
	// 	return;
	// }
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
//	wifi_get_macaddr(SOFTAP_IF, &originalMac[0]);
//	wifi_set_macaddr(SOFTAP_IF, &broadcastMac[0]);
//	wifi_get_macaddr(STATION_IF, &originalMac[0]);
//	wifi_set_macaddr(STATION_IF, &broadcastMac[0]);

	// TODO Sauberes INIT;  Channel muss passend gesetzt werden
	bool b = WiFi.softAP("sender", "sendersender", 1, false);
	Serial.println("ESPNOW: SoftAP " + String(b));
	WiFi.mode(WIFI_AP_STA ); // Must be AP_STA!


	int ret = esp_now_init();
	if (ret == 0) {
		Logger::log(LogLevel::ERROR, "ESP_NOW: OK");
	} else {
		Logger::log(LogLevel::ERROR, "ESP_NOW: FAILED! ERR: " + String(ret));
	}
	delay(10);

	esp_now_peer_info_t slave;
  	memset(&slave, 0, sizeof(slave));
  	for (int i = 0; i < 6; ++i)
	    slave.peer_addr[i] = 0xFF;
  	slave.channel = 1; // pick a channel
  	slave.encrypt = 0; // no encryption

/*  	slave.encrypt = 1; // no encryption
	slave.priv = key;*/


  	const esp_now_peer_info_t *peer = &slave;
  	const uint8_t *peer_addr = slave.peer_addr;
  	esp_err_t addStatus = esp_now_add_peer(peer);	
	Logger::log(LogLevel::ERROR, "Added: " + String(addStatus));
	//esp_now_add_peer(broadcastMac, 1, key, 16);
//	esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

	esp_now_register_send_cb(CmdReceiverESPNOW::cb_MsgSend);
	esp_now_register_recv_cb(CmdReceiverESPNOW::cb_MsgReceived);


	//	esp_now_register_recv_cb([](uint8_t *mac, uint8_t *data, uint8_t len) {
	// TODO Nachricht zum Array hinzufügen

	//		  memcpy(&rcvmac, mac, 6);
	//		  memcpy(&rcvData, data, dataLength);
	//		  for (int i=0; i < dataLength; i++) {
	//			  Serial.print(String(rcvData[i]));
	//			  Serial.print(" - ");
	//		  }
	//	      Serial.print("   from   ");
	//		  for (int i=0; i < 6; i++) {
	//			  Serial.print(String(mac[i]));
	//			  Serial.print(" - ");
	//		  }
	//	      Serial.print(".\r\n");
	//	      back = true;
	//	  });
	readyForSend = true;
}

CmdReceiverESPNOW::~CmdReceiverESPNOW() {
}


int CmdReceiverESPNOW::loop() {
	if (rolle == unknown) {
		return 1000;
	}
	// TODO Auswertung der empfangnen Pakete
	// TODO Weitersendung der empfangenn PaketeR
	if (receivedQuery.size() > 0) {
		msg* m = receivedQuery.pop();
		Serial.println(rolename + ": Received");
		Serial.println(rolename + ": Typ: " + String(m->typ));
		Serial.println(rolename + ": ID: " + String(m->id));
		Serial.println(rolename + ": Hops: " + String(m->hops));
		if (rolle == repeater) {
			m->hops++;
			sendQuery.add(m);
		} else {
			free(m);
		}
	}
	readyForSend = true;
	if (readyForSend && sendQuery.size() > 0) {
		readyForSend = false;
		msg* m = sendQuery.pop();
		Serial.println(rolename + ": Sending");
		esp_err_t result = esp_now_send(broadcastMac, reinterpret_cast<unsigned char*>(m), msgsize);
		// Serial.println("Sendresult: " +String(result));
		// Serial.println(esp_err_to_name(result));
		free(m);
	}
	if (rolle == master && (last + 1000) < millis()) {
		// Jede Sekunde eine neue Message
		msg* m = new msg;
		m->hops = 0;
		m->id = esp_random();
		m->typ = messagetyps::internal;
		for (int i = 0; i < 20; i++) {
			m->msg[i] = i;
		}
		sendQuery.add(m);
		last = millis();
	}
	return 10;

}

void CmdReceiverESPNOW::cb_MsgReceived(const uint8_t *mac_addr, const uint8_t *data, int len) {
	if (len != msgsize) {
		Serial.println("Rejecting ESPNOW Message: " + String(msgsize) + "/" + String(len));
		return;
	}
	const msg* recevied = reinterpret_cast<const msg*>(data);
//	Serial.println("Type: " + String(recevied->typ));
	msg* m = new msg;
	memcpy(m, recevied,len);
	instance->receivedQuery.add(m);
//	memcpy(msg&, data&, len);
	Serial.println("Type: " + String(m->typ));
//	msg* m = new msg;
	//Serial.println("Adding received message");
	//memcpy(m, data, len);
	
	// Serial.println("Received");
	// for (int i=0; i < len; i++) {
	// 	Serial.print(data[i], HEX);
	// 	Serial.print(" ");
	// }
	// Serial.println(" <=> ");
	// for (int i=0; i < 6; i++) {
	// 	Serial.print(mac_addr[i], HEX);
	// 	Serial.print(" ");
	// }
	// Serial.println();
}

void CmdReceiverESPNOW::cb_MsgSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
	Serial.println("Sended");
	readyForSend = true;
}



bool CmdReceiverESPNOW::readyForSend = false;
int  CmdReceiverESPNOW::msgsize = sizeof(CmdReceiverESPNOW::msg);
CmdReceiverESPNOW* CmdReceiverESPNOW::instance = NULL;

#endif 
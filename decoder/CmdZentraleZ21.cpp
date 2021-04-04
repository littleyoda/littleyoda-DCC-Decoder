/*
 * CmdZentraleZ21.cpp
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#include "CmdZentraleZ21.h"
#include <stdint.h>
#include "Controller.h"
#include "Logger.h"
#include "Consts.h"
#include "Utils.h"


CmdZentraleZ21::CmdZentraleZ21(Controller* c) : Z21Format(c) {
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting Z21 Zentrale ...");
	cnt = c;
	udp = new WiFiUDP();
}

int CmdZentraleZ21::loop() {
	// Check for UDP
	if (Utils::isWifiConnected()) {
		if (!udpSetup) {
			udp->begin(localPort);
			udpSetup = true;
		}
		currentDestIP = udp->remoteIP();
		currentDestPort = udp->remotePort();
		int cb = udp->parsePacket();
		if (cb > 0) {
			while (udp->available() > 0)  {
				doReceive();
			}
		}
		currentDestPort = 0;
	}
	long int time = millis();
	if ((timeout > 0) && ((time - timeout) > emergencyStopTimeout)) {
		Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21 Zentrale Timeout");
		cnt->emergencyStop(Consts::SOURCE_Z21SERVER, true);
		timeout = 0;
	}

	if (clients.size() > 0) {
		// Goal => each client receive two package pre second 
		int intervall = 500 / clients.size();
		if  ((time - lastBroadcastTime) > intervall) {
			if (broadcastClientIdx >= clients.size()) {
				broadcastClientIdx = 0;
			}
			Z21Clients* c = clients.get(broadcastClientIdx);
			if (c->ids.size()> 0) {
				if (c->broadcastIdx >= c->ids.size()) {
					c->broadcastIdx = 0; 
				}
				currentDestIP = c->ip;
				currentDestPort = c->port;
				sendLocoInfoToClient(c->ids.get(c->broadcastIdx));
				currentDestPort = 0;
				c->broadcastIdx++;
			}
			broadcastClientIdx++;
			lastBroadcastTime = time;
		}
	}
	return 2;

}





void CmdZentraleZ21::doReceive() {
	udp->read(pb, 1);
	int cb = (int) pb[0];
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	if (udp->available() < cb-1) {
		Logger::log(LogLevel::INFO, "Unvollständiges UDP-Paket " + String(cb -1) + "/" + udp->available());
		udp->read(&pb[1], udp->available());
		return;
	}
	int ret = udp->read(&pb[1], cb - 1);
	if (debugEnabled) {
		Serial.print("RECV: ");
		printPacketBuffer(cb);
	}
	resetTimeout();

	if (parseClient2Server(pb, ret + 1)) {
		return;
	}

	unsigned char SET_BROADCASTFLAGS[4] = {0x08, 0x00, 0x50, 0x00};
	if (cb>= 8 && memcmp(SET_BROADCASTFLAGS, pb, 4) == 0) {
		uint32_t value = pb[7] << 24 | pb[6] << 16 | pb[5] << 8 | pb[4];
		// Serial.println("Brodcast-Request" + String(value));
		// Serial.println(value, BIN);
		// Serial.println(value, HEX);
		IPAddress ip = udp->remoteIP();
		uint16_t port =  udp->remotePort();
		Z21Clients* c = getClient(ip, port);
		if (c != NULL) {
			c->flags = value;
		} else {
			c = new Z21Clients();
			c->port = port;
			c->ip = ip;
			c->flags = value;
			clients.add(c);
			Logger::log(LogLevel::DEBUG, "Client Added " + ip.toString());
		}
		return;
	}
	Serial.print("Unbekannt: ");
	printPacketBuffer(cb);
}



/**
 * Sorgt dafür, dass für die aktuellen Client die Loc-Addr als Broadcast Ziel
 * hinzugefügt wird
 * 
 */
void CmdZentraleZ21::adjustBroadcast(int addr) {
	//	Add id to Z21Clients List
	Z21Clients* c = getClient(udp->remoteIP(), udp->remotePort());
	if (c == NULL) {
		IPAddress ip = udp->remoteIP();
		uint16_t port =  udp->remotePort();
		c = new Z21Clients();
		c->port = port;
		c->ip = ip;
		c->flags = 0;
		clients.add(c);
   	}
	// Add Addr if not already in the list
	for (int i = 0; i < c->ids.size(); i++) {
		int id = c->ids.get(i);			
		if (id == addr) {
			return;
		}
	}
	c->ids.add(addr);
}

CmdZentraleZ21::~CmdZentraleZ21() {
}

void CmdZentraleZ21::resetTimeout() {
	timeout = millis();
}


void CmdZentraleZ21::printPacketBuffer(int size) {
	Serial.print("Hex: ");
	for (int i = 0; i < size; i++) {
		Serial.print(Utils::getHex(pb[i]));
		Serial.print(" ");
	}
	Serial.print(" [");
	Serial.print(udp->remoteIP());
	Serial.print(" => ");
#ifdef defined(udp->destinationIP())
	Serial.print(udp->destinationIP());
#else
	Serial.print("???.???.???.??? ");
#endif

	Serial.println("]");
}


void CmdZentraleZ21::emergencyStop() {
	cnt->emergencyStop(Consts::SOURCE_Z21SERVER, true);
}


Z21Clients* CmdZentraleZ21::getClient(IPAddress ip, uint16_t port) {
	for (int i = 0; i < clients.size(); i++) {
		Z21Clients* c = clients.get(i);
		if (c->port == port && c->ip == ip) {
			return c;
		}
	}
	return NULL;
}


void CmdZentraleZ21::send() {
	if (currentDestPort == 0) {
		Logger::log(LogLevel::ERROR, "[Z21] Ziel für send() nicht festgelegt.");
		return;
	}
	udp->beginPacket(currentDestIP, currentDestPort);
//	printPacketBuffer(pb[0]);
	udp->write(pb, pb[0]);
	int ret = udp->endPacket(); 
	if (ret == 0) {
		Logger::log(LogLevel::ERROR, "UDP Send Error");
	}
	delay(1);
}


/*
	über INotify

	=> Information an alle relevanten Clients verteilen
 */
void CmdZentraleZ21::DCCSpeed(int addr, int speed, int direction, int SpeedSteps, int source) {
	if (source == Consts::SOURCE_Z21SERVER) {
		return;
	}
	if (clients.size() == 0) {
		return;
	}

	LocData* data = controller->getLocData(addr);
	memset(pb, 0, packetBufferSize);
	pb[0] = 14;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xEF;
	pb[5] = (addr >> 8) & 0x3F;
	if (addr >= 128) {
		pb[5] += 0b11000000;
	}

	pb[6] = addr & 255;

	pb[7] = 4; // 128 Fahrstufen
	unsigned int v = (data->speed & 127);
	// Adjust to match NmraDCC Schema
	if (v == Consts::SPEED_STOP) {
		v = 0;
	} else if (v == Consts::SPEED_EMERGENCY) {
		v = 1;
	}
	pb[8] = v | ((data->direction == -1) ? 0 : 128);
	pb[9] = ((data->status >> 1) & 15) | (data->status & 1) << 4;
	pb[10] = (data->status >> 5) & 255;
	pb[11] = (data->status >> 13) & 255;
	pb[12] = (data->status >> 21) & 255;

	pb[13] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7] ^ pb[8] ^ pb[9] ^ pb[10] ^ pb[11] ^ pb[12];
	
	for (int i = 0; i < clients.size(); i++) {
		Z21Clients* c = clients[i];
		for (int idx = 0; idx < c->ids.size(); idx++) {
			int id = c->ids[idx];
			if (id == addr)  {
				udp->beginPacket(c->ip, c->port);
				udp->write(pb, pb[0]);
				int ret = udp->endPacket(); 
				if (ret == 0) {
					Logger::log(LogLevel::ERROR, "UDP Send Error -- DCCSpeed ClientIdx: " + String(i));
				}
				delay(2);
			}
		}
	}
}


// String CmdZentraleZ21::getInternalStatus(String key) {
// 	String out = Z21Format::getInternalStatus(key);
// 	if (key == "clients" || key == "*") {
// 		out += "clients\n";
// 		for (int i = 0; i < clients.size(); i++) {
// 			Z21Clients* c = clients[i];
// 			out += c->ip.toString() + ":" + String(c->port);
// 			for (int idx = 0; idx < c->ids.size(); idx++) {
// 				int id = c->ids[idx];
// 				out += "," + String(id);
// 			}
// 			out += "\n";
// 		}
// 	}
// 	Serial.println(out);
// 	return out;
// }


// void CmdZentraleZ21::handleBroadcast() {
// 	uint32_t value = pb[7] << 24 | pb[6] << 16 | pb[5] << 8 | pb[4];
// 	Serial.println("Broadcast ");
// 	Serial.println(value, HEX);
// 	if ((value & 0x1) > 0) {
// 		bool found = false;
// 		for (int idx = 0; idx < clients->size(); idx++) {
// 			ClientsStruct* c = clients->get(idx);
// 			if (c->addr == udp->remoteIP() && c->port == udp->remotePort()) {
// 				found = true;
// 				break;
// 			}
// 		}
// 		if (found) {
// 			Serial.println("Bereits aktiv");
// 		} else {
// 			Serial.println("Adding");
// 			ClientsStruct* c = new ClientsStruct();
// 			c->port = udp->remotePort();
// 			c->addr = udp->remoteIP();
// 			clients->add(c);
// 		}
// 	}
// }



void CmdZentraleZ21::sendSetTurnout(String sid, String status) {
	int id = sid.toInt();
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x09;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x43;
	pb[5] = id >> 8;
	pb[6] = id & 255;
	unsigned char out = 0;
	TurnOutData* data = controller->getTurnOutData(id);
	if (data->direction == 0) {
		out = 1;
	} else if (data->direction == 1) {
		out = 2;
	} else {
		out = 0;
	}
	Serial.println("Richtung: " + String(data->direction));
	pb[7] = out;
	pb[8] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7];
	for (int idx = 0; idx < clients.size(); idx++) {
			Z21Clients* c = clients.get(idx);
			Serial.println("Sending " + c->ip.toString());
	//		Serial.println(String(addr) + " " + String(v));
			udp->beginPacket(c->ip, c->port);
			udp->write(pb, pb[0]);
			int ret = udp->endPacket();
			if (ret == 0) {
				Logger::log(LogLevel::ERROR, "UDP Send Error -- Turnout ClientIdx: " + String(idx));
			}
	}
}

void CmdZentraleZ21::sendSetSensor(uint16_t id, uint8_t status) {
	// TODO
}



void CmdZentraleZ21::sendDCCSpeed(int addr, LocData* data) {
	createDCCSpeedCmd(addr, data);

	for (int idx = 0; idx < clients.size(); idx++) {
			Z21Clients* c = clients.get(idx);
			Serial.println("Sending " + c->ip.toString());
			Serial.println(String(addr) + " " + String(data->speed));
			udp->beginPacket(c->ip, c->port);
			udp->write(pb, pb[0]);
			int ret = udp->endPacket();
			if (ret == 0) {
				Logger::log(LogLevel::ERROR, "UDP Send Error -- sendDCCSpeed: " + String(idx));
			}
			delay(1);
	}
};


void CmdZentraleZ21::sendDCCFun(int addr, LocData* data, unsigned int changedBit) {
	Serial.println("Changed Bit");
	sendDCCSpeed(addr,data);
}
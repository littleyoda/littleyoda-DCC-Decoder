/*
 * CmdReceiverZ21Wlan.cpp
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#include "CmdReceiverZ21Wlan.h"
#include <stdint.h>
#include "Controller.h"

CmdReceiverZ21Wlan::CmdReceiverZ21Wlan(Controller* c, uint8_t ip1, uint8_t ip2,
		uint8_t ip3, uint8_t ip4) :
		CmdReceiverBase(c) {

	z21Server = new IPAddress(ip1, ip2, ip3, ip4);
	udp = new WiFiUDP();
	udp->begin(localPort);
	enableBroadcasts();
}

void CmdReceiverZ21Wlan::loop() {
	// Check for UDP
	int cb = udp->parsePacket();
	if (cb != 0) {
		doReceive(cb);
	}
	// Scheduler for Requests
	long int time = millis();
	if (time - lastTime > 1000) {
		lastTime = time;
//	    if (loopstatus < turnoutCount) {
//	      requestTurnoutInfo(turnoutAddr[loopstatus]);
//	    } else {
		enableBroadcasts();
//	    }
//	    loopstatus++;
//	    if (loopstatus >= turnoutCount + 1) {
//	      loopstatus = 0;
//	    }
	}

}

void CmdReceiverZ21Wlan::handleTurnout() {
	// Check if this is a message for us
	int id = packetBuffer[5] << 8 | packetBuffer[6];
	int status = packetBuffer[7];
	// see z21 documention, why this hack is neccessary
	int output = -1;
	if (status == 1) {
		output = 0;
	} else if (status == 2) {
		output = 1;
	}

	controller->notifyTurnout(id, output);
}

void CmdReceiverZ21Wlan::handleDCCSpeed(unsigned int locoid) {
	unsigned int fahrstufen = packetBuffer[7] & 7;
	if (fahrstufen == 0) {
		fahrstufen = 14;
	} else if (fahrstufen == 2) {
		fahrstufen = 28;
	} else if (fahrstufen == 4) {
		fahrstufen = 128;
	}

	int richtung = (packetBuffer[8] & 128) == 0 ? -1 : 1;
	;
	unsigned int v = (packetBuffer[8] & 127);
	controller->notifyDCCSpeed(locoid, v, richtung, fahrstufen);
}

void CmdReceiverZ21Wlan::handleFunc(unsigned int locoid) {
	unsigned long func = (((packetBuffer[9] & 16) > 0) ? 1 : 0)
			+ ((packetBuffer[9] & 15) << 1) + ((packetBuffer[10]) << 5)
			+ ((packetBuffer[11]) << (8 + 5))
			+ ((packetBuffer[12]) << (16 + 5));
	controller->notifyDCCFun(locoid, 0, 29, func);
}

void CmdReceiverZ21Wlan::doReceive(int cb) {
//	Serial.print("packet received, length=");
	Serial.println(cb);
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	udp->read(packetBuffer, cb);

	// Check if this is a TURNOUT_INFO
	boolean turnOutInfoPaket = cb == 9 && packetBuffer[0] == 0x09
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x43;
	if (turnOutInfoPaket) {
		handleTurnout();
		return;
	}
	boolean loco_info = cb >=7 && packetBuffer[0] >= 8
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0xEF;
	if (loco_info) {
		unsigned int locoid = ((packetBuffer[5] & 0x3f) << 8) + packetBuffer[6];
		handleDCCSpeed(locoid);
		handleFunc(locoid);
		return;
	}
//	// Print unknown packets:
//	Serial.println("New Paket:");
//	for (int i = 0; i < cb; i++) {
//		Serial.print(packetBuffer[i], HEX);
//		Serial.print(" ");
//	}
//	Serial.println();
}

/**
 * Fragt den Weichenstatus der Weiche bei der Z21 an
 */
void CmdReceiverZ21Wlan::requestTurnoutInfo(int addr) {
	if (addr == -1) {
		return;
	}
	//Serial.println("Sending Turnout-Info Request");
	memset(packetBuffer, 0, packetBufferSize);

	packetBuffer[0] = 0x08;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x40;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0x43;
	packetBuffer[5] = addr >> 8;
	packetBuffer[6] = addr & 255;
	packetBuffer[7] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6];

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, 8);
	udp->endPacket();
}

/**
 * Sendet der Z21 einen BroadcastRequest
 */
void CmdReceiverZ21Wlan::enableBroadcasts() {
//  Serial.println("Sending Broadcast Request");
	memset(packetBuffer, 0, packetBufferSize);

	// 0x08 0x00 0x50 0x00 0x01 0x00 0x01 0x00
	// 2.16 LAN_SET_BROADCASTFLAGS
	packetBuffer[0] = 0x08;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x50;
	packetBuffer[3] = 0x00;

	// Flags = 0x00010001 (Little Endian)
	// 0x01 and 0x010000
	packetBuffer[4] = 0x01;
	packetBuffer[5] = 0x00;
	packetBuffer[6] = 0x01;
	packetBuffer[7] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, 8);
	udp->endPacket();
}

/**
 * Sendet einen Weichenbefehl, der über das Webinterface ausgelöst wurde, an die Z21
 */
void CmdReceiverZ21Wlan::sendSetTurnout(String id, String status) {
	//Serial.println("Sending Set Turnout");
	//addToLog("Web-Request ID: " + String(id) + " Status: " + status);
	int statuscode = 0;
	if (status == "1") {
		statuscode = 1;
	}
	memset(packetBuffer, 0, packetBufferSize);

	// 5.2 LAN_X_SET_TURNOUT
	packetBuffer[0] = 0x09;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x40;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0x53;
	packetBuffer[5] = 0x00;
	packetBuffer[6] = id.toInt();
	packetBuffer[7] = 0xA8 | statuscode;
	packetBuffer[8] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6]
			^ packetBuffer[7];

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, 9);
	udp->endPacket();
}

//int getTurnoutIdx(int id) {
//  int i;
//  for (i = 0; i < turnoutCount; i++) {
//    if (turnoutAddr[i] == id) {
//      return i;
//    }
//  }
//  return -1;
//}

CmdReceiverZ21Wlan::~CmdReceiverZ21Wlan() {
	// TODO Auto-generated destructor stub
}


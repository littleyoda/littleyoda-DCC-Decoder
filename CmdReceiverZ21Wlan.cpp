/*
 * CmdReceiverZ21Wlan.cpp
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#include "CmdReceiverZ21Wlan.h"
#include <stdint.h>
#include "Controller.h"
#include "Logger.h"
#include "Consts.h"
#include "Utils.h"

CmdReceiverZ21Wlan::CmdReceiverZ21Wlan(Controller* c, String ip) :
z21PaketParser(c) {
	Logger::getInstance()->addToLog("Starting Z21 Wlan Receiver ...");
	if (ip == NULL) {
		z21Server = new IPAddress(192, 168, 0, 111);
	} else {
		z21Server = new IPAddress();
		z21Server->fromString(ip);

	}
	Logger::getInstance()->addToLog("Using: " + String(z21Server->toString()));
	udp = new WiFiUDP();
	udp->begin(localPort);
	enableBroadcasts();
}

int CmdReceiverZ21Wlan::loop() {
	// Check for UDP
	int cb = udp->parsePacket();
	if (cb > 0) {
		while (udp->available() > 0) {
			doReceive();
		}
	}
	long int time = millis();
	if ((timeout > 0) && ((time - timeout) > emergencyStopTimeout)) {
		Logger::getInstance()->addToLog("Z21 wlan Timeout");
		controller->emergencyStop(Consts::SOURCE_INTERNAL);
		timeout = 0;
	}

	// Scheduler for Requests
	if (time - lastTime > (cmdSendTime)) {
		lastTime = time;
		if (loopStatus == -1) {
			switch (subloopstatus) {
				case 0: sendFrimwareVersionRequest(); break;
				case 1: sendXGetStatus(); break;
				case 2: enableBroadcasts(); break;
				case 3: sendCfg12Request(); break;
			}
			subloopstatus++;
			if (subloopstatus > 3) {
				subloopstatus = 0;
			}
		} else {
			INotify::requestInfo* ri = requestList->get(loopStatus);
			if (ri->art == INotify::requestInfo::LOCO) {
				requestLocoInfo(ri->id);
			} else if (ri->art == INotify::requestInfo::TURNOUT) {
				requestTurnoutInfo(ri->id);
			}
		}
		loopStatus++;
		if ((requestList == NULL && loopStatus == 0)
				|| (requestList != NULL && loopStatus == requestList->size())) {
			loopStatus = firstLoopStatus;
		}
	}
	return 2;

}

void CmdReceiverZ21Wlan::requestRailcom() {
	memset(packetBuffer, 0, packetBufferSize);

	packetBuffer[0] = 0x07;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x89;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0x01;
	packetBuffer[5] = 0;
	packetBuffer[6] = 0;
	packetBuffer[7] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6];

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, 8);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::doReceive() {

	udp->read(packetBuffer, 1);
	int cb = (int) packetBuffer[0];
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	int ret = udp->read(&packetBuffer[1], cb - 1);
	parser(packetBuffer, cb);
	resetTimeout();

	//
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

	// Flags = 0x00 01 00 01 (Little Endian)
	// 0x01 and 0x01 00 00
	packetBuffer[4] = 0x01;
	packetBuffer[5] = 0x00;
	packetBuffer[6] = 0x01;
	packetBuffer[7] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
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
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

CmdReceiverZ21Wlan::~CmdReceiverZ21Wlan() {
}

void CmdReceiverZ21Wlan::resetTimeout() {
	if (timeout == 0) {
		Serial.println("Message from Z21 received!");
	}
	timeout = millis();
}

void CmdReceiverZ21Wlan::sendLanGetSerialNumber() {
	memset(packetBuffer, 0, packetBufferSize);

	// 5.2 LAN_X_SET_TURNOUT
	packetBuffer[0] = 0x04;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x10;
	packetBuffer[3] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::sendCfg12Request() {
	memset(packetBuffer, 0, packetBufferSize);
	// undokumentiert;	04:00:12:00
	packetBuffer[0] = 0x04;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x12;
	packetBuffer[3] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::sendCfg16Request() {
	memset(packetBuffer, 0, packetBufferSize);
	//	undokumentiert; 04:00:16:00
	packetBuffer[0] = 0x04;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x16;
	packetBuffer[3] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::sendXGetStatus() {
	memset(packetBuffer, 0, packetBufferSize);

	// 2.4 LAN_X_GET_STATUS
	packetBuffer[0] = 0x07;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x40;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0x21;
	packetBuffer[5] = 0x24;
	packetBuffer[6] = 0x05;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::sendGetBroadcastFlags() {
	memset(packetBuffer, 0, packetBufferSize);

	// 4.1 LAN_X_GET_LOCO_INFO
	packetBuffer[0] = 0x04;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x51;
	packetBuffer[3] = 0x00;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();

}

void CmdReceiverZ21Wlan::sendFrimwareVersionRequest() {
	memset(packetBuffer, 0, packetBufferSize);

	// 4.1 LAN_X_GET_LOCO_INFO
	packetBuffer[0] = 0x07;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x40;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0xF1;
	packetBuffer[5] = 0x0A;
	packetBuffer[6] = 0xFB;

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

void CmdReceiverZ21Wlan::requestLocoInfo(int addr) {
	memset(packetBuffer, 0, packetBufferSize);

	// 4.1 LAN_X_GET_LOCO_INFO
	packetBuffer[0] = 0x09;
	packetBuffer[1] = 0x00;
	packetBuffer[2] = 0x40;
	packetBuffer[3] = 0x00;

	packetBuffer[4] = 0xE3;
	packetBuffer[5] = 0xF0;

	packetBuffer[6] = addr >> 8;
	if (addr >= 128) {
		packetBuffer[6] += 0b11000000;
	}

	packetBuffer[7] = addr & 0xFF;
	packetBuffer[8] = packetBuffer[4] ^ packetBuffer[5] ^ packetBuffer[6]
																	   ^ packetBuffer[7];

	udp->beginPacket(*z21Server, localPort);
	udp->write(packetBuffer, packetBuffer[0]);
	udp->endPacket();
}

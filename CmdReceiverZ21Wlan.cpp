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

CmdReceiverZ21Wlan::CmdReceiverZ21Wlan(Controller* c, const char* ip) :
		CmdReceiverBase(c) {
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
		while (udp->available() > 0)  {
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
	if (time - lastTime > (emergencyStopTimeout / 4)) {
		lastTime = time;
		if (loopStatus == -4) {
			requestRailcom();
		} else if (loopStatus == -3) {
			sendXGetStatus();
		} else if (loopStatus == -2) {
			enableBroadcasts();
		} else if (loopStatus == -1) {
			sendCfg12Request();
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

	controller->notifyTurnout(id, output, 1);
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
	unsigned int v = (packetBuffer[8] & 127);
	// Adjust to match NmraDCC Schema
	if (v == 0) {
		v = Consts::SPEED_STOP;
	} else if (v == 1) {
		v = Consts::SPEED_EMERGENCY;
	}
	controller->notifyDCCSpeed(locoid, v, richtung, fahrstufen, 1);
}

void CmdReceiverZ21Wlan::handleFunc(unsigned int locoid) {
	unsigned long func = (((packetBuffer[9] & 16) > 0) ? 1 : 0)
			+ ((packetBuffer[9] & 15) << 1) + ((packetBuffer[10]) << 5)
			+ ((packetBuffer[11]) << (8 + 5))
			+ ((packetBuffer[12]) << (16 + 5));
	controller->notifyDCCFun(locoid, 0, 29, func, 1);
}

void CmdReceiverZ21Wlan::doReceive() {

	udp->read(packetBuffer, 1);
	int cb = (int) packetBuffer[0];
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	int ret = udp->read(&packetBuffer[1], cb - 1);
//	printPacketBuffer(cb);
	resetTimeout();

	boolean getSerialNumber = cb >= 8 && packetBuffer[0] == 0x08
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x10
			&& packetBuffer[3] == 0x00;
	if (getSerialNumber) {
		return;
	}

	// LAN_X_STATUS_CHANGED
	boolean statusChanged = cb >= 8 && packetBuffer[0] == 0x08
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x62
			&& packetBuffer[5] == 0x22;
	if (statusChanged) {
		// Test if the status has been changed
		if (packetBuffer[6] == lastZ21Status) {
			return;
		}
		//csEmergencyStop
		if ((packetBuffer[6] & 0x01) > 0) {
			Logger::getInstance()->addToLog("Z21: csEmergencyStop");
			emergencyStop();
		}
		//csTrackVoltageOff
		if ((packetBuffer[6] & 0x02) > 0) {
			Logger::getInstance()->addToLog("Z21: csTrackVoltageOff");
			emergencyStop();
		}
		//csShortCircuit
		if ((packetBuffer[6] & 0x04) > 0) {
			Logger::getInstance()->addToLog("Z21: csShortCircuit");
			Serial.println("Short Circuit");
			// TODO
		}
		//csProgrammingModeActive
		if ((packetBuffer[6] & 0x20) > 0) {
			Logger::getInstance()->addToLog("Z21: csProgrammingModeActive");
			Serial.println("ProgrammingMode");
			// TODO
		}
		lastZ21Status = packetBuffer[6];
		return;
	}

	// Check if this is a TURNOUT_INFO
	boolean turnOutInfoPaket = cb >= 9 && packetBuffer[0] == 0x09
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x43;
	if (turnOutInfoPaket) {
		handleTurnout();
		return;
	}

	boolean loco_info = cb >= 7 && packetBuffer[0] >= 8
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0xEF;
	if (loco_info) {
		unsigned int locoid = ((packetBuffer[5] & 0x3f) << 8) + packetBuffer[6];
		handleDCCSpeed(locoid);
		handleFunc(locoid);
		return;
	}

	// LAN_X_BC_TRACK_POWER_OFF
	boolean poweroff = cb >= 7 && packetBuffer[0] == 0x07
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x61
			&& packetBuffer[5] == 0x00 && packetBuffer[6] == 0x61;
	if (poweroff) {
		emergencyStop();
		return;
	}

	// LAN_X_BC_STOPPED
	boolean emergencystop = cb >= 7 && packetBuffer[0] == 0x07
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x81
			&& packetBuffer[5] == 0x00 && packetBuffer[6] == 0x81;
	if (emergencystop) {
		emergencyStop();
		return;
	}

	// LAN_X_BC_TRACK_POWER_ON
	boolean poweron = cb >= 7 && packetBuffer[0] == 0x07
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0x61
			&& packetBuffer[5] == 0x01 && packetBuffer[6] == 0x60;
	if (poweron) {
		// todo
		return;
	}

	boolean cfg12 = cb >= 0xE && packetBuffer[0] == 0x0E
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x12
			&& packetBuffer[3] == 0x00;

	if (cfg12) {
		int offset = 0;
		if ((packetBuffer[11] & 4) == 0) {
			offset  = 0;
		} else {
			offset = 4;
		}
		if (turnoutOffset != offset) {
			Logger::getInstance()->addToLog("Turnout Offset: " + String(offset));
			turnoutOffset = offset;
		}
		return;
	}
	boolean cfg16 = cb >= 0x14 && packetBuffer[0] == 0x14
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x16
			&& packetBuffer[3] == 0x00;
	if (cfg16) {
		return;
	}

	boolean railcom = cb >= 0x11 && packetBuffer[0] >= 0x11
				&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x88
				&& packetBuffer[3] == 0x00;
	if (railcom) {
		handleRailcomdata();
		return;
	}

	Serial.print("Unbekannt: ");
	printPacketBuffer(cb);
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

void CmdReceiverZ21Wlan::printPacketBuffer(int size) {
	for (int i = 0; i < size; i++) {
		Serial.print(Utils::getHex(packetBuffer[i]));
		Serial.print(" ");
	}
	Serial.println();
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

void CmdReceiverZ21Wlan::handleRailcomdata() {
	int lokid = packetBuffer[0 + 4] + (packetBuffer[1 + 4] << 8);
//	Serial.println("LokID: " + String(lokid));
//	Serial.println("  Options: " + String(packetBuffer[9 + 4]));
//	Serial.println("  Speed: " + String(packetBuffer[10 + 4]));
//	Serial.println("  QoS: " + String(packetBuffer[11 + 4]));
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

void CmdReceiverZ21Wlan::emergencyStop() {
	controller->emergencyStop(Consts::SOURCE_WLAN);
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

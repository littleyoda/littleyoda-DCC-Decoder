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

CmdZentraleZ21::CmdZentraleZ21(Controller* c, const char* ip) :
		CmdReceiverBase(c) {
	Logger::getInstance()->addToLog("Starting Z21 Zentrale ...");
//	if (ip == NULL) {
//		z21Server = new IPAddress(192, 168, 0, 111);
//	} else {
//		z21Server = new IPAddress();
//		z21Server->fromString(ip);
//
//	}
//	Logger::getInstance()->addToLog("Z21-Server Using: " + String(z21Server->toString()));
	udp = new WiFiUDP();
	udp->begin(localPort);
}

int CmdZentraleZ21::loop() {
	// Check for UDP
	int cb = udp->parsePacket();
	if (cb != 0) {
		doReceive(cb);
	}
	long int time = millis();
	if ((timeout > 0) && ((time - timeout) > emergencyStopTimeout)) {
		Serial.println("Timeout");
		Logger::getInstance()->addToLog("Z21 Zentrale Timeout");
		controller->emergencyStop(Consts::SOURCE_Z21SERVER);
		timeout = 0;
	}

//	// Scheduler for Requests
//	if (time - lastTime > (emergencyStopTimeout / 4)) {
//		lastTime = time;
//		if (loopStatus == -3) {
//			sendXGetStatus();
//		} else if (loopStatus == -2) {
//			enableBroadcasts();
//		} else if (loopStatus == -1) {
//			sendCfg12Request();
//		} else {
//			ActionBase::requestInfo* ri = requestList->get(loopStatus);
//			if (ri->art == ActionBase::requestInfo::LOCO) {
//				requestLocoInfo(ri->id);
//			} else if (ri->art == ActionBase::requestInfo::TURNOUT) {
//				requestTurnoutInfo(ri->id);
//			}
//		}
//		loopStatus++;
//		if ((requestList == NULL && loopStatus == 0)
//				|| (requestList != NULL && loopStatus == requestList->size())) {
//			loopStatus = firstLoopStatus;
//		}
//	}
	return 2;

}

void CmdZentraleZ21::handleSetLocoFunc(unsigned int locoid) {
	unsigned int value = (pb[8] >> 6) & 1;
	unsigned int bit = pb[8] & 63;
	controller->notifyDCCFun(locoid, bit, value, Consts::SOURCE_Z21SERVER);
}

void CmdZentraleZ21::doReceive(int cb) {
	// TODO Handle Packets with multiple Blocks in one UDP Paket
	// TODO Read the first two bytes to determine the size
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	udp->read(pb, cb);
	resetTimeout();

	boolean LAN_X_GET_STATUS = cb >= 7
			&& pb[0] == 0x07 && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] == 0x21 && pb[5] == 0x24 && pb[6] == 0x05;
			;
	if (LAN_X_GET_STATUS) {
		//Serial.println("Status request");
		return;
	}

	boolean X_GET_LOCO_INFO = cb >=9
			&& pb[0] == 0x09 && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] == 0xe3 && pb[5] == 0xf0;;
	if (X_GET_LOCO_INFO) {
		int id = ((pb[6] & 0x3F) << 8) + pb[7];
		sendLocoInfoToClient(id);
		return;
	}

	unsigned char GET_LOCOMODE[4] = { 0x06, 0x00, 0x60, 0x00 };
	if (cb>= 6 && memcmp(GET_LOCOMODE, pb, 4) == 0) {
		handleLocoMode();
		return;
	}

	unsigned char GET_FIRMWARE[7] = {0x07, 0x00, 0x40, 0x00, 0xf1, 0x0a, 0xfb};
	if (cb>= 7 && memcmp(GET_FIRMWARE, pb, 4) == 0) {
		Serial.println("Get Firmware-Version");
		return;
	}

	unsigned char SET_LOCO_DRIVE[5] = {0x0a, 0x00, 0x40, 0x00, 0xe4};
	if (cb>= 10 && memcmp(SET_LOCO_DRIVE, pb, 5) == 0) {
		unsigned int locoid = ((pb[6] & 0x3f) << 8) + pb[7];
		if (pb[5] == 0xF8) {
			handleSetLocoFunc(locoid);
			sendLocoInfoToClient(locoid);
			return;
		}
		if ((pb[5] >> 4) == 1) {
			handleSetLoco(locoid);
			sendLocoInfoToClient(locoid);
			return;
		}

	}
	printPacketBuffer(cb);
}

void CmdZentraleZ21::handleSetLoco(int locoid) {
	unsigned int fahrstufen = pb[5] & 7;
	if (fahrstufen == 0) {
		fahrstufen = 14;
	} else if (fahrstufen == 2) {
		fahrstufen = 28;
	} else if (fahrstufen == 3) {
		fahrstufen = 128;
	} else {
		fahrstufen = 999;
	}

	int richtung = (pb[8] & 128) == 0 ? -1 : 1;
	unsigned int v = (pb[8] & 127);
	// Adjust to match NmraDCC Schema
	if (v == 0) {
		v = Consts::SPEED_STOP;
	} else if (v == 1) {
		v = Consts::SPEED_EMERGENCY;
	}
	controller->notifyDCCSpeed(locoid, v, richtung, fahrstufen, 1);
	sendLocoInfoToClient(locoid);
}

void CmdZentraleZ21::sendLocoInfoToClient(int addr) {
	LocData* data = controller->getLocData(addr);
	if (data->speedsteps == 0) {
		data->speedsteps = 128;
	}
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
	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
// DEBUG zum decode the packet
//
//	unsigned int locoid = ((pb[5] & 0x3f) << 8) + pb[6];
//	unsigned int fahrstufen = pb[7] & 7;
//	if (fahrstufen == 0) {
//		fahrstufen = 14;
//	} else if (fahrstufen == 2) {
//		fahrstufen = 28;
//	} else if (fahrstufen == 4) {
//		fahrstufen = 128;
//	}
//
//	int richtung = (pb[8] & 128) == 0 ? -1 : 1;
//	v = (pb[8] & 127);
//	// Adjust to match NmraDCC Schema
//	if (v == 0) {
//		v = Consts::SPEED_STOP;
//	} else if (v == 1) {
//		v = Consts::SPEED_EMERGENCY;
//	}
//	Serial.println("Gesendet: D:" + String(richtung) + " Speed:" + String(v) + "/ Steps" + String(fahrstufen));
}

void CmdZentraleZ21::handleLocoMode() {
	// Reuse request
	pb[0] = 0x07;
	pb[6] = 0; // DCC Format

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
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
	Serial.println();
}


void CmdZentraleZ21::emergencyStop() {
	controller->emergencyStop(Consts::SOURCE_Z21SERVER);
}


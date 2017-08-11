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

CmdZentraleZ21::CmdZentraleZ21(Controller* c) :
		CmdReceiverBase(c) {
	Logger::getInstance()->addToLog("Starting Z21 Zentrale ...");
	udp = new WiFiUDP();
	udp->begin(localPort);
}

int CmdZentraleZ21::loop() {
	// Check for UDP
	int cb = udp->parsePacket();
	if (cb > 0) {
		while (udp->available() > 0)  {
			doReceive();
		}
	}
	long int time = millis();
	if ((timeout > 0) && ((time - timeout) > emergencyStopTimeout)) {
		Serial.println("Timeout");
		Logger::getInstance()->addToLog("Z21 Zentrale Timeout");
		controller->emergencyStop(Consts::SOURCE_Z21SERVER);
		timeout = 0;
	}
	return 2;

}

void CmdZentraleZ21::sendFirmware() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 9;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xF3;
	pb[5] = 0x0A;
	pb[6] = 3;
	pb[7] = 0X44;
	pb[8] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7];
	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::sendHwinfo() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x0C;
	pb[1] = 0x00;
	pb[2] = 0x1A;
	pb[3] = 0x00;

	pb[4] = 0x03;
	pb[5] = 0x02; // 202 => Smartrail
	pb[6] = 0x00;
	pb[7] = 0x00;

	pb[8] = 20;
	pb[9] = 0x01;
	pb[10] = 0x00;
	pb[11] = 0x00;
	pb[12] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7]^ pb[8] ^ pb[9] ^ pb[10]^ pb[11];
	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::handleGetStatus() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x14;
	pb[1] = 0x00;
	pb[2] = 0x84;
	pb[3] = 0x00;

	pb[4] = 0x00; //0
	pb[5] = 0x01;

	pb[6] = 0x00; // 2
	pb[7] = 0x01;

	pb[8] = 0x00; // 4
	pb[9] = 0x01;

	pb[10] = 0x10; // 6 Temperatur
	pb[11] = 0x00;

	pb[12] = 0x00;// 8
	pb[13] = 0x10;

	pb[14] = 0x00; // 10
	pb[15] = 0x10;

	pb[16] = 0x00; // 12
	pb[17] = 0x00; // 13
	pb[18] = 0x00; // 14
	pb[19] = 0x00; // 15

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::handleGetSerial() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0x10;
	pb[3] = 0x00;

	pb[4] = 0x01;
	pb[5] = 0x02;
	pb[6] = 0x03;
	pb[7] = 0x04;

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::handleGetVersion() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x09;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x63;
	pb[5] = 0x21;
	pb[6] = 0x30;
	pb[7] = 0x12;
	pb[8] = 0x60;

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::handleBIB() {
	// Unvollständig
	int len = pb[4] - 0xe5;
	int id = ((pb[6] & 0x3F) << 8) + pb[7];
	Serial.print(String((pb[8]) + 1) + "/" + String(pb[9]) + "Length: " + String(len) + " ID: " + String(id) + " Name :");
	for (int i = 10; i < 10 + len; i++) {
		Serial.print((char)pb[i]);
	}
	Serial.println("<");
}

void CmdZentraleZ21::sendStatusChanged() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x62;
	pb[5] = 0x22;
	if (controller->isEmergency()) {
		pb[6] = 1;
	} else {
		pb[6] = 0;
	}
	pb[7] = pb[4] ^ pb[5] ^ pb[6];

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

void CmdZentraleZ21::handleTurnInfoRequest(int id) {
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
	pb[7] = out;
	pb[8] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7];

	udp->beginPacket(udp->remoteIP(), udp->remotePort());
	udp->write(pb, pb[0]);
	udp->endPacket();
}

int CmdZentraleZ21::handleSetTurnInfoRequest() {
	int id = (pb[5] << 8) + pb[6];
	boolean status = (bit_is_set(pb[7], 0));
	boolean actived = (bit_is_set(pb[7], 3));
	if (actived == 0) {
		return -1;
	}
	controller->notifyTurnout(id, status, Consts::SOURCE_Z21SERVER);
	return id;
}

void CmdZentraleZ21::handleSetLocoFunc(unsigned int locoid) {
	unsigned int value = (pb[8] >> 6);
	unsigned int bit = pb[8] & 63;
	if (value > 1) {
		// Modus "umschalten"
		LocData* ld = controller->getLocData(locoid);
		if (bit_is_set(ld->status, bit)) {
			value = 0;
		} else {
			value = 1;
		}
	}
	controller->notifyDCCFun(locoid, bit, value, Consts::SOURCE_Z21SERVER);
}

void CmdZentraleZ21::doReceive() {
	udp->read(pb, 1);
	int cb = (int) pb[0];
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	int ret = udp->read(&pb[1], cb - 1);
//	printPacketBuffer(cb);
	resetTimeout();

	boolean LAN_X_GET_STATUS = cb >= 7
			&& pb[0] == 0x07 && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] == 0x21 && pb[5] == 0x24 /** && pb[6] == 0x05 */;
			;
	if (LAN_X_GET_STATUS) {
		sendStatusChanged();
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

	// Get Serial
	unsigned char GET_SERIAL[4] = { 0x04, 0x00, 0x10, 0x00 };
	if (cb>= 4 && memcmp(GET_SERIAL, pb, 4) == 0) {
		handleGetSerial();
		return;
	}

	unsigned char GET_FIRMWARE[7] = {0x07, 0x00, 0x40, 0x00, 0xf1, 0x0a, 0xfb};
	if (cb>= 7 && memcmp(GET_FIRMWARE, pb, 7) == 0) {
		sendFirmware();
		return;
	}


	unsigned char GET_HWINFO[4] = {0x04, 0x00, 0x1A, 0x00};
	if (cb>= 4 && memcmp(GET_HWINFO, pb, 4) == 0) {
		sendHwinfo();
		return;
	}

	unsigned char SET_BROADCASTFLAGS[4] = {0x08, 0x00, 0x50, 0x00};
	if (cb>= 4 && memcmp(SET_BROADCASTFLAGS, pb, 4) == 0) {
		Serial.println("Brodcast-Request");
		return;
	}

	unsigned char GET_VERSION[7] = {0x07, 0x00, 0x40, 0x00, 0x21, 0x21, 0x00};
	if (cb>= 7 && memcmp(GET_VERSION, pb, 7 - 1) == 0) {
		handleGetVersion();
		return;
	}


	// LAN_X_GET_STATUS
	unsigned char GETSTATUS[7] = {0x07, 0x00, 0x40, 0x00, 0x21,0x24, 0x00};
	if (cb>=7 && memcmp(GETSTATUS, pb, 7) == 0) {
		handleGetStatus();
		return;
	}

	unsigned char GETSTATUS2[7] = {0x07, 0x00, 0x40, 0x00, 0x21,0x24, 0xd2};
	if (cb>=7 && memcmp(GETSTATUS2, pb, 7) == 0) {
		handleGetStatus();
		return;
	}

	// LAN_SYSTEMSTATE_GETDATA
	unsigned char SYSTEMSTATE_GETDATA[4] = {0x04, 0x00, 0x85, 0x00};
	if (cb>= 4 && memcmp(SYSTEMSTATE_GETDATA, pb, 4) == 0) {
		handleGetStatus();
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

	// Die WLAN MAUS sendet entgegen der Spezifikation dieses Paket an die Z21
	unsigned char LAN_X_GET_TURNOUT_INFO[5] = {0x09, 0x00, 0x40, 0x00, 0x43};
	if (cb>= 9 && memcmp(LAN_X_GET_TURNOUT_INFO, pb, 5) == 0) {
		int id = (pb[5] << 8) + pb[6];
		handleTurnInfoRequest(id);
		return;
	}

	// Lok-Bibliothek 0xE5 up to 0xEF.
	boolean LOKBIB_RECEIVED = cb >= 0x0c
			&& pb[0] >= 0x0c && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] >= 0xE6 && pb[4] <= 0xEF
			&& pb[5] == 0xf1;
	if (LOKBIB_RECEIVED) {
		handleBIB();
		return;
	}

	// Weichenbefehle
	unsigned char LAN_X_SET_TURNOUT_INFO[5] = {0x09, 0x00, 0x40, 0x00, 0x53};
	if (cb>= 8 && memcmp(LAN_X_SET_TURNOUT_INFO, pb, 5) == 0) {
		int id = handleSetTurnInfoRequest();
		if (id != -1) {
			handleTurnInfoRequest(id);
		}
		return;
	}

	Serial.print("Unbekannt: ");
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
	Serial.print(" [");
	Serial.print(udp->remoteIP());
	Serial.print(" => ");
	Serial.print(udp->destinationIP());
	Serial.println("]");
}


void CmdZentraleZ21::emergencyStop() {
	controller->emergencyStop(Consts::SOURCE_Z21SERVER);
}


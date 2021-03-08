/*
 * Z21FORMAT.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Z21Format.h"
#include "Controller.h"
#include "Logger.h"
#include "Consts.h"
#include "SpeedKonverter.h"
#include "Utils.h"

Z21Format::Z21Format(Controller* c) :  CmdReceiverBase(c), CmdSenderBase() {
//	controller = c;
	for (int i = 0; i < 30; i++)
		pb[i] = 0;

}

Z21Format::~Z21Format() {
}


bool Z21Format::parseServer2Client(unsigned char pb[], int cb) {

	// ------------------------------------------------
	// ------------------------------------------------
	// ------------------------------------------------
	//  From Z21 Zentrale
	// ------------------------------------------------
	// ------------------------------------------------
	// ------------------------------------------------
	boolean getSerialNumber = cb >= 8 && pb[0] == 0x08
			&& pb[1] == 0x00 && pb[2] == 0x10
			&& pb[3] == 0x00;
	if (getSerialNumber) {
		return true;
	}

	// LAN_X_STATUS_CHANGED
	boolean statusChanged = cb >= 8 && pb[0] == 0x08
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0x62
			&& pb[5] == 0x22;
	if (statusChanged) {
		// Test if the status has been changed
		if (pb[6] == lastZ21Status) {
			return true;
		}
		bool newstatus = false;
		//csEmergencyStop
		if ((pb[6] & 0x01) > 0) {
			Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21: csEmergencyStop");
			newstatus = true;
		}
		//csTrackVoltageOff
		if ((pb[6] & 0x02) > 0) {
			Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21: csTrackVoltageOff");
			newstatus = true;
		}
		//csShortCircuit
		if ((pb[6] & 0x04) > 0) {
			Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21: csShortCircuit");
			newstatus = true;
		}
		if (newstatus != z21EmergencyStop) {
			z21EmergencyStop = newstatus;
			emergencyStop(z21EmergencyStop);
		}
		//csProgrammingModeActive
		if ((pb[6] & 0x20) > 0) {
			Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21: csProgrammingModeActive");
		}
		lastZ21Status = pb[6];
		return true;
	}

	// Check if this is a TURNOUT_INFO
	boolean turnOutInfoPaket = cb >= 9 && pb[0] == 0x09
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0x43;
	if (turnOutInfoPaket) {
		handleTurnout(pb);
		return true;
	}


	// LAN_X_LOCO_INFO 
	boolean loco_info = cb >= 7 && pb[0] >= 8
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0xEF;
	if (loco_info) {
		unsigned int locoid = ((pb[5] & 0x3f) << 8) + pb[6];
		handleDCCSpeed(locoid, pb);
		handleFunc(locoid, pb);
		return true;
	}

	// LAN_X_BC_TRACK_POWER_OFF
	boolean poweroff = cb >= 7 && pb[0] == 0x07
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0x61
			&& pb[5] == 0x00 && pb[6] == 0x61;
	if (poweroff) {
		emergencyStop(true);
		return true;
	}

	// LAN_X_BC_STOPPED
	boolean emergencystop = cb >= 7 && pb[0] == 0x07
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0x81
			&& pb[5] == 0x00 && pb[6] == 0x81;
	if (emergencystop) {
		emergencyStop(true);
		return true;
	}

	// LAN_X_BC_TRACK_POWER_ON
	boolean poweron = cb >= 7 && pb[0] == 0x07
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0x61
			&& pb[5] == 0x01 && pb[6] == 0x60;
	if (poweron) {
		emergencyStop(true);
		return true;
	}

	boolean cfg12 = cb >= 0xE && pb[0] == 0x0E
			&& pb[1] == 0x00 && pb[2] == 0x12
			&& pb[3] == 0x00;

	if (cfg12) {
		int offset = 0;
		if ((pb[11] & 4) == 0) {
			offset  = 0;
		} else {
			offset = 4;
		}
		if (turnoutOffset != offset) {
			Logger::getInstance()->addToLog(LogLevel::INFO, "Turnout Offset: " + String(offset));
			turnoutOffset = offset;
		}
		return true;
	}
	boolean cfg16 = cb >= 0x14 && pb[0] == 0x14
			&& pb[1] == 0x00 && pb[2] == 0x16
			&& pb[3] == 0x00;
	if (cfg16) {
		return true;
	}

	boolean railcom = cb >= 0x11 && pb[0] >= 0x11
				&& pb[1] == 0x00 && pb[2] == 0x88
				&& pb[3] == 0x00;
	if (railcom) {
		handleRailcomdata(pb);
		return true;
	}

	boolean firmware = cb == 9 && pb[0] == 9
			&& pb[1] == 0x00 && pb[2] == 0x40
			&& pb[3] == 0x00 && pb[4] == 0xF3
			&& pb[5] == 0x0A;
	if (firmware) {
		// TODO printPacketBuffer("Firmware", pb, cb);
		handleFirmware(pb);
		return true;
	}
	return false;

}

bool Z21Format::parseClient2Server(unsigned char pb[], int cb) {

	// ------------------------------------------------
	// --------- From Clients -------------------------	
	// ------------------------------------------------
	// ------------------------------------------------
	// ------------------------------------------------
	boolean LAN_X_GET_STATUS = cb >= 7
			&& pb[0] == 0x07 && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] == 0x21 && pb[5] == 0x24 /** && pb[6] == 0x05 */;
			;
	if (LAN_X_GET_STATUS) {
		//debugMsg("LAN_X_GET_STATUS");
		sendStatusChanged(); 
		return true;
	}

	// Get Serial
	unsigned char GET_SERIAL[4] = { 0x04, 0x00, 0x10, 0x00 };
	if (cb>= 4 && memcmp(GET_SERIAL, pb, 4) == 0) {
		debugMsg("Get Serial Request");
		handleGetSerial();
		return true;
	}

	unsigned char GET_FIRMWARE[7] = {0x07, 0x00, 0x40, 0x00, 0xf1, 0x0a, 0xfb};
	if (cb>= 7 && memcmp(GET_FIRMWARE, pb, 7) == 0) {
		sendFirmware(); 
		return true;
	}


	unsigned char GET_HWINFO[4] = {0x04, 0x00, 0x1A, 0x00};
	if (cb>= 4 && memcmp(GET_HWINFO, pb, 4) == 0) {
		sendHwinfo(); 
		return true;
	}

	unsigned char GET_VERSION[7] = {0x07, 0x00, 0x40, 0x00, 0x21, 0x21, 0x00};
	if (cb>= 7 && memcmp(GET_VERSION, pb, 7 - 1) == 0) {
		handleGetVersion(); 
		return true;
	}

	// LAN_X_GET_STATUS
	unsigned char GETSTATUS[7] = {0x07, 0x00, 0x40, 0x00, 0x21,0x24, 0x00};
	if (cb>=7 && memcmp(GETSTATUS, pb, 7) == 0) {
		handleGetStatus(); 
		return true;
	}

	unsigned char GETSTATUS2[7] = {0x07, 0x00, 0x40, 0x00, 0x21,0x24, 0xd2};
	if (cb>=7 && memcmp(GETSTATUS2, pb, 7) == 0) {
		handleGetStatus(); 
		return true;
	}

	// LAN_SYSTEMSTATE_GETDATA
	unsigned char SYSTEMSTATE_GETDATA[4] = {0x04, 0x00, 0x85, 0x00};
	if (cb>= 4 && memcmp(SYSTEMSTATE_GETDATA, pb, 4) == 0) {
		handleGetStatus(); 
		return true;
	}

	// Lok-Bibliothek 0xE5 up to 0xEF. -- Not implemented
	boolean LOKBIB_RECEIVED = cb >= 0x0c
			&& pb[0] >= 0x0c && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] >= 0xE6 && pb[4] <= 0xEF
			&& pb[5] == 0xf1;
	if (LOKBIB_RECEIVED) {
	//	handleBIB();
		return true;
	}

	// Die WLAN MAUS sendet entgegen der Spezifikation dieses Paket an die Z21
	unsigned char LAN_X_GET_TURNOUT_INFO[5] = {0x09, 0x00, 0x40, 0x00, 0x43};
	if (cb>= 9 && memcmp(LAN_X_GET_TURNOUT_INFO, pb, 5) == 0) {
		int id = (pb[5] << 8) + pb[6];
		handleTurnInfoRequest(id); 
		return true;
	}


	// Weichenbefehle
	unsigned char LAN_X_SET_TURNOUT_INFO[5] = {0x09, 0x00, 0x40, 0x00, 0x53};
	if (cb>= 8 && memcmp(LAN_X_SET_TURNOUT_INFO, pb, 5) == 0) {
		int id = handleSetTurnInfoRequest();
		if (id != -1) {
			handleTurnInfoRequest(id); 
		}
		return true;
	}

	// Fordert den Status einer Lok an
	boolean X_GET_LOCO_INFO = cb >=9
			&& pb[0] == 0x09 && pb[1] == 0x00
			&& pb[2] == 0x40 && pb[3] == 0x00
			&& pb[4] == 0xe3 && pb[5] == 0xf0;;
	if (X_GET_LOCO_INFO) {
		int id = ((pb[6] & 0x3F) << 8) + pb[7];
		debugMsg("Get Loco Info " + String(id));
		sendLocoInfoToClient(id);
		return true;
	}

	unsigned char GET_LOCOMODE[4] = { 0x06, 0x00, 0x60, 0x00 }; 
	if (cb>= 6 && memcmp(GET_LOCOMODE, pb, 4) == 0) {
		debugMsg("LAN_GET_LOCOMODE (MM or DCC)");
		handleLocoMode(); 
		return true;
	}

	//  Fahrstufe oder Funktionen eines Lok-Decoders wird geändert
	unsigned char SET_LOCO_DRIVE[5] = {0x0a, 0x00, 0x40, 0x00, 0xe4};
	if (cb>= 10 && memcmp(SET_LOCO_DRIVE, pb, 5) == 0) {
		unsigned int locoid = ((pb[6] & 0x3f) << 8) + pb[7];
		if (pb[5] == 0xF8) {
			debugMsg("SET_LOCO_DRIVE (FUNC) " + String(locoid));
			handleSetLocoFunc(locoid);
		} else  if ((pb[5] >> 4) == 1) {
			debugMsg("SET_LOCO_DRIVE (SPEED) " + String(locoid));
			handleSetLoco(locoid);
		} else {
			debugMsg("SET_LOCO_DRIVE (UNBEKANNT) " + String(locoid));
		}
		sendLocoInfoToClient(locoid);
		return true;
	}

	//printPacketBuffer("Unbekanntes Paket", pb, cb);
	return false;
}

void Z21Format::debugMsg(String s) {
	if (debugEnabled) {
		Logger::log(LogLevel::TRACE, "[Z21]" + s);
	}
}

int Z21Format::handleSetTurnInfoRequest() {
	int id = (pb[5] << 8) + pb[6];
	boolean status = (bit_is_set(pb[7], 0));
	boolean actived = (bit_is_set(pb[7], 3));
	if (actived == 0) {
		return -1;
	}
	controller->notifyTurnout(id, status, Consts::SOURCE_Z21SERVER);
	return id;
}


void Z21Format::printPacketBuffer(String msg, unsigned char pb[], int size) {
	String out = msg + ": ";
	for (int i = 0; i < size; i++) {
		out += (Utils::getHex(pb[i]));
		out += " ";
	}
	Logger::log(LogLevel::DEBUG, out);
}


void Z21Format::emergencyStop(bool status) {
	controller->emergencyStop(Consts::SOURCE_WLAN, status);
}


void Z21Format::handleTurnout(unsigned char pb[]) {
	// Check if this is a message for us
	int id = pb[5] << 8 | pb[6];
	int status = pb[7];
	// see z21 documention, why this hack is neccessary
	int output = -1;
	if (status == 1) {
		output = 0;
	} else if (status == 2) {
		output = 1;
	}

	controller->notifyTurnout(id, output, 1);
}

void Z21Format::handleDCCSpeed(unsigned int locoid, unsigned char pb[]) {
	unsigned int fahrstufen = pb[7] & 7;
	if (fahrstufen == 0) {
		fahrstufen = 14;
	} else if (fahrstufen == 2) {
		fahrstufen = 28;
	} else if (fahrstufen == 3) {
		fahrstufen = 128;
	} else if (fahrstufen == 4) {
		fahrstufen = 128;
	}

	int richtung = (pb[8] & 128) == 0 ? -1 : 1;
	unsigned int origv = pb[8] & 127;
	unsigned int v = SpeedKonverter::fromExternal(fahrstufen, origv);
//	Serial.println("z21 handleDCCSpeed Loc:" + String(locoid) + " FS " + String(fahrstufen) + " Orig: " + String(origv) + " V: " + String(v));
	controller->notifySpeeSteps(locoid, fahrstufen);
	controller->notifyDCCSpeed(locoid, v, richtung, 1);
}

void Z21Format::handleFunc(unsigned int locoid, unsigned char pb[]) {
	unsigned long func = (((pb[9] & 16) > 0) ? 1 : 0)
			+ ((pb[9] & 15) << 1) + ((pb[10]) << 5)
			+ ((pb[11]) << (8 + 5))
			+ ((pb[12]) << (16 + 5));
	controller->notifyDCCFun(locoid, 0, 29, func, 1);
}


void Z21Format::handleRailcomdata(unsigned char pb[]) {
//	int lokid = pb[0 + 4] + (pb[1 + 4] << 8);
//	Serial.println("LokID: " + String(lokid));
//	Serial.println("  Options: " + String(pb[9 + 4]));
//	Serial.println("  Speed: " + String(pb[10 + 4]));
//	Serial.println("  QoS: " + String(pb[11 + 4]));
}


void Z21Format::handleFirmware(unsigned char pb[]) {
	unsigned int msb = pb[6];
	unsigned int v =(unsigned int) pb[7];
	unsigned int lsbA = v >> 4;
	unsigned int lsbB = v & 15;
	if (firmwareVersion == 0) {
		firmwareVersion = msb * 100 + lsbA * 10 + lsbB;
		//Logger::getInstance()->addToLog(LogLevel::INFO, "Fimrware Version: " + String(firmwareVersion));
	}
}

void Z21Format::sendFirmware() {
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
	send();
}

void Z21Format::sendHwinfo() {
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
	send();	
}

void Z21Format::handleGetStatus() {
	// 2.18 LAN_SYSTEMSTATE_DATACHANGED
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
	send();
}

void Z21Format::handleGetSerial() {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0x10;
	pb[3] = 0x00;

	pb[4] = 0x01;
	pb[5] = 0x02;
	pb[6] = 0x03;
	pb[7] = 0x04;
	send();
}

void Z21Format::handleGetVersion() {
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
	send();
}


void Z21Format::sendStatusChanged() {
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
	send();
}

void Z21Format::handleTurnInfoRequest(int id) {
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
	send();
}


void Z21Format::handleLocoMode() {
	// Reuse request
	pb[0] = 0x07;
	pb[6] = 0; // DCC Format
	send();
}

void Z21Format::send() {

}


void Z21Format::handleSetLocoFunc(unsigned int locoid) {
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


void Z21Format::handleSetLoco(int locoid) {
	unsigned int fahrstufen = pb[5] & 7;
	if (fahrstufen == 0) {
		fahrstufen = 14;
	} else if (fahrstufen == 2) {
		fahrstufen = 28;
	} else if (fahrstufen == 3) {
		fahrstufen = 128;
	} else if (fahrstufen == 4) {
		fahrstufen = 128;
	}
	int richtung = (pb[8] & 128) == 0 ? -1 : 1;	
	int v = (pb[8] & 127);
	v = SpeedKonverter::fromExternal(fahrstufen, v);
	Serial.println("[z21] Received: " + String(richtung) + " " + String(v) + "/" + String(fahrstufen));
	controller->notifySpeeSteps(locoid, fahrstufen);
	controller->notifyDCCSpeed(locoid, v, richtung, 1);
	sendLocoInfoToClient(locoid);
	adjustBroadcast(locoid);
}

void Z21Format::adjustBroadcast(int addr) {

}

void Z21Format::sendLocoInfoToClient(int addr) {
	// TODO
	LocData* data = controller->getLocData(addr);
	if (data->speedsteps == 0) {
		data->speedsteps = Consts::DEFAULTSPEEDSTEPS;
	}
	debugMsg("Send Loco Info ID: " + String(addr) + " " + String(data->direction) + " " + String(data->speed) + "/" + String(data->speedsteps) + " F:" + String(data->status));

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
	// TODO
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
	send();
	adjustBroadcast(addr);
}

String Z21Format::getName() {
	return "z21";
}

// String Z21Format::getInternalStatus(String key) {
// 	String out = "";
// 	// if (key == "clients" || key == "*") {
// 	// 	out += "clients\n";
// 	// }
// 	return out;
// }


void Z21Format::sendCfg12Request() {
	memset(pb, 0, packetBufferSize);
	// undokumentiert;	04:00:12:00
	pb[0] = 0x04;
	pb[1] = 0x00;
	pb[2] = 0x12;
	pb[3] = 0x00;
	send();
}

void Z21Format::sendCfg16Request() {
	memset(pb, 0, packetBufferSize);
	//	undokumentiert; 04:00:16:00
	pb[0] = 0x04;
	pb[1] = 0x00;
	pb[2] = 0x16;
	pb[3] = 0x00;
	send();
}

void Z21Format::sendXGetStatus() {
	memset(pb, 0, packetBufferSize);

	// 2.4 LAN_X_GET_STATUS -- get Status der Zentrale
	pb[0] = 0x07;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x21;
	pb[5] = 0x24;
	pb[6] = 0x05;
	send();
}

void Z21Format::sendFrimwareVersionRequest() {
	memset(pb, 0, packetBufferSize);

	// 4.1 LAN_X_GET_LOCO_INFO
	pb[0] = 0x07;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xF1;
	pb[5] = 0x0A;
	pb[6] = 0xFB;
	send();
}


/**
 * Fragt den Weichenstatus der Weiche bei der Z21 an
 */
void Z21Format::requestTurnoutInfo(int addr) {
	if (addr == -1) {
		return;
	}
	//Serial.println("Sending Turnout-Info Request");
	memset(pb, 0, packetBufferSize);

	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x43;
	pb[5] = addr >> 8;
	pb[6] = addr & 255;
	pb[7] = pb[4] ^ pb[5] ^ pb[6];

	send();
}

/**
 * Sendet der Z21 einen BroadcastRequest
 */
void Z21Format::enableBroadcasts() {
	//  Serial.println("Sending Broadcast Request");
	memset(pb, 0, packetBufferSize);

	// 0x08 0x00 0x50 0x00 0x01 0x00 0x01 0x00
	// 2.16 LAN_SET_BROADCASTFLAGS
	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0x50;
	pb[3] = 0x00;

	// Flags = 0x00 01 00 01 (Little Endian)
	// 0x01 and 0x01 00 00
	pb[4] = 0x01; // 0x00000001
	pb[5] = 0x00;
	pb[6] = 0x01; // Client bekommt nun LAN_X_LOCO_INFO, 
	pb[7] = 0x00;
	send();
}

/**
 * Sendet einen Weichenbefehl, der über das Webinterface ausgelöst wurde, an die Z21
 */
void Z21Format::sendSetTurnout(String id, String status) {
	//Serial.println("Sending Set Turnout");
	//addToLog("Web-Request ID: " + String(id) + " Status: " + status);
	int statuscode = 0;
	if (status == "1") {
		statuscode = 1;
	}
	memset(pb, 0, packetBufferSize);

	// 5.2 LAN_X_SET_TURNOUT
	pb[0] = 0x09;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0x53;
	pb[5] = 0x00;
	pb[6] = id.toInt();
	pb[7] = 0xA8 | statuscode;
	pb[8] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7];
	send();
}


void Z21Format::requestLocoInfo(int addr) {
	memset(pb, 0, packetBufferSize);

	// 4.1 LAN_X_GET_LOCO_INFO
	pb[0] = 0x09;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xE3;
	pb[5] = 0xF0;

	pb[6] = addr >> 8;
	if (addr >= 128) {
		pb[6] += 0b11000000;
	}

	pb[7] = addr & 0xFF;
	pb[8] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7];
	send();
}


void Z21Format::setTrackPower(bool b) {
	memset(pb, 0, packetBufferSize);

	// SET TRACKER POWER OFF/ON
	pb[0] = 0x07;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;
	pb[4] = 0x21;

	if (b) {
		pb[5] = 0x81;	// Power on  -- E false 
	} else {
		pb[5] = 0x80;	// Power off -- E True
	}
	pb[6] = pb[4] ^ pb[5];
	send();
}


void Z21Format::createDCCSpeedCmd(int addr, LocData* data) {
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
}


/* 
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
*/


/* 
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
*/


// void Z21Format::handleBIB() {
// 	// Unvollständig
// 	int len = pb[4] - 0xe5;
// 	int id = ((pb[6] & 0x3F) << 8) + pb[7];
// 	Serial.print(String((pb[8]) + 1) + "/" + String(pb[9]) + "Length: " + String(len) + " ID: " + String(id) + " Name :");
// 	for (int i = 10; i < 10 + len; i++) {
// 		Serial.print((char)pb[i]);
// 	}
// 	Serial.println("<");
// }


// void CmdReceiverZ21Wlan::requestRailcom() {
// 	memset(pb, 0, packetBufferSize);

// 	pb[0] = 0x07;
// 	pb[1] = 0x00;
// 	pb[2] = 0x89;
// 	pb[3] = 0x00;

// 	pb[4] = 0x01;
// 	pb[5] = 0;
// 	pb[6] = 0;
// 	pb[7] = pb[4] ^ pb[5] ^ pb[6];

// 	udp->beginPacket(*z21Server, localPort);
// 	udp->write(pb, 8);
// 	udp->endPacket();
// }

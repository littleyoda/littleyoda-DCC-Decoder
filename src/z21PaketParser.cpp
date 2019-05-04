/*
 * z21PaketParser.cpp
 *
 *  Created on: 01.01.2018
 *      Author: sven
 */

#include "z21PaketParser.h"
#include "Utils.h"

z21PaketParser::z21PaketParser(Controller* c) : CmdReceiverBase(c) {

}

z21PaketParser::~z21PaketParser() {
}

void z21PaketParser::parser(unsigned char packetBuffer[], int cb) {
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
		handleTurnout(packetBuffer);
		return;
	}

	boolean loco_info = cb >= 7 && packetBuffer[0] >= 8
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0xEF;
	if (loco_info) {
		unsigned int locoid = ((packetBuffer[5] & 0x3f) << 8) + packetBuffer[6];
		handleDCCSpeed(locoid, packetBuffer);
		handleFunc(locoid, packetBuffer);
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
		handleRailcomdata(packetBuffer);
		return;
	}

	boolean firmware = cb == 9 && packetBuffer[0] == 9
			&& packetBuffer[1] == 0x00 && packetBuffer[2] == 0x40
			&& packetBuffer[3] == 0x00 && packetBuffer[4] == 0xF3
			&& packetBuffer[5] == 0x0A;
	if (firmware) {
		printPacketBuffer(packetBuffer, cb);
		handleFirmware(packetBuffer);
		return;
	}
	Serial.print("Unbekannt: ");
	printPacketBuffer(packetBuffer, cb);

}


void z21PaketParser::printPacketBuffer(unsigned char packetBuffer[], int size) {
	for (int i = 0; i < size; i++) {
		Serial.print(Utils::getHex(packetBuffer[i]));
		Serial.print(" ");
	}
	Serial.println();
}


void z21PaketParser::emergencyStop() {
	controller->emergencyStop(Consts::SOURCE_WLAN);
}


void z21PaketParser::handleTurnout(unsigned char packetBuffer[]) {
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

void z21PaketParser::handleDCCSpeed(unsigned int locoid, unsigned char packetBuffer[]) {
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

void z21PaketParser::handleFunc(unsigned int locoid, unsigned char packetBuffer[]) {
	unsigned long func = (((packetBuffer[9] & 16) > 0) ? 1 : 0)
			+ ((packetBuffer[9] & 15) << 1) + ((packetBuffer[10]) << 5)
			+ ((packetBuffer[11]) << (8 + 5))
			+ ((packetBuffer[12]) << (16 + 5));
	controller->notifyDCCFun(locoid, 0, 29, func, 1);
}


void z21PaketParser::handleRailcomdata(unsigned char packetBuffer[]) {
	int lokid = packetBuffer[0 + 4] + (packetBuffer[1 + 4] << 8);
//	Serial.println("LokID: " + String(lokid));
//	Serial.println("  Options: " + String(packetBuffer[9 + 4]));
//	Serial.println("  Speed: " + String(packetBuffer[10 + 4]));
//	Serial.println("  QoS: " + String(packetBuffer[11 + 4]));
}


void z21PaketParser::handleFirmware(unsigned char packetBuffer[]) {
	unsigned int msb = packetBuffer[6];
	unsigned int v =(unsigned int) packetBuffer[7];
	unsigned int lsbA = v >> 4;
	unsigned int lsbB = v & 15;
	if (firmwareVersion == 0) {
		firmwareVersion = msb * 100 + lsbA * 10 + lsbB;
		Logger::getInstance()->addToLog("Fimrware Version: " + String(firmwareVersion));
	}
}


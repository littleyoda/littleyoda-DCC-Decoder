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
#include "SpeedKonverter.h"

CmdReceiverZ21Wlan::CmdReceiverZ21Wlan(Controller* c, String ip) :
 Z21Format(c) {
	setModulName("Z21 Client");
	Logger::getInstance()->addToLog(LogLevel::INFO, "Starting Z21 Wlan Receiver ...");
	if (ip == NULL) {
		z21Server = new IPAddress(192, 168, 0, 111);
	} else {
		z21Server = new IPAddress();
		z21Server->fromString(ip);

	}
	udp = new WiFiUDP();
	Logger::getInstance()->addToLog(LogLevel::INFO, "Using " + String(z21Server->toString()) + " as Z21 Adress");
}

int CmdReceiverZ21Wlan::loop() {
	long int time = millis();
	if ((timeout > 0) && ((time - timeout) > emergencyStopTimeout)) {
		Logger::getInstance()->addToLog(LogLevel::WARNING, "Z21 wlan Timeout");
		controller->emergencyStop(Consts::SOURCE_INTERNAL);
		timeout = 0;
		timeouts++;
	}

	if (!Utils::isWifiConnected()) {
		return 40;
	}
	if (!udpSetup) {
		udp->begin(localPort);
		udpSetup = true;
		enableBroadcasts();
	}

	// Check for UDP
	int cb = udp->parsePacket();
	if (cb > 0) {
		while (udp->available() > 0) {
			doReceive();
		}
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


void CmdReceiverZ21Wlan::doReceive() {
	udp->read(pb, 1);
	int cb = (int) pb[0];
	if (cb > packetBufferSize) {
		cb = packetBufferSize;
	}
	int ret = udp->read(&pb[1], cb - 1);
	parseServer2Client(pb, cb);
	resetTimeout();
}


/**
 * Sendet einen Gleis besetzt-Signal, der über ein GBM ausgelöst wurde, an die Z21 Loconet
 * 
 * // 5.2 LAN_LOCONET_FROM_LAN
 * 
 * 
 * OPC_INPUT_REP 0xB2; 
 * General SENSOR Input
 * <0xB2>, <IN1>, <IN2>, <CHK>
 *         <IN1>=<0,A6,A5,A4,A3,A2,A1,A0>, 7 ls adr bits. A1,A0 select 1 of 4 inputs pairs in a DS54
 *         <IN2>=<0,X,I,L- A10,A9,A8,A7>Report/status bits and 4 MS adr bits.
 */
void CmdReceiverZ21Wlan::sendSetSensor(uint16_t id, uint8_t status) {
	Serial.println("Sending Set Track");
	Logger::getInstance()->addToLog(LogLevel::INFO, "Sensor state ID: " + String(id) + " Status: " + status);

  	id = id-1;
  
	byte AddrL = ( id >> 1 ) & 0x7F ;
	byte AddrH = 0;
	if (bitRead(id,7)) bitSet(AddrH, 0);
	if (bitRead(id,8)) bitSet(AddrH, 1);
	if (bitRead(id,9)) bitSet(AddrH, 2);
	if (bitRead(id,10)) bitSet(AddrH, 3);
	if (status) bitSet(AddrH, 4);
	if (id % 2) bitSet(AddrH, 5);
	bitSet(AddrH, 6);
  
	memset(pb, 0, 8);

	pb[0] = 0x08;
	pb[1] = 0x00;
	pb[2] = 0xa2;
	pb[3] = 0x00;

	pb[4] = 0xb2;
	pb[5] = AddrL;
	pb[6] = AddrH;  
  	pb[7] = 0xFF ^ pb[4] ^ pb[5] ^ pb[6];
	send();
 }

void CmdReceiverZ21Wlan::sendDCCSpeed(int addr, LocData* data) {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0X0A;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xE4;
	
	unsigned int speed = 0;
	if (data->speedsteps == 128) {
		pb[5] = 0x13;
		speed = SpeedKonverter::fromInternal(SpeedKonverter::repDCC128, data->speed);
	} else if (data->speedsteps == 28) {
		pb[5] = 0x12;
		speed = SpeedKonverter::fromInternal(SpeedKonverter::repDCC28, data->speed);
	} else if (data->speedsteps == 14) {
		pb[5] = 0x10;
		speed = SpeedKonverter::fromInternal(SpeedKonverter::repDCC14, data->speed);
	} else {
		Logger::log(LogLevel::ERROR, "Unsupported Speedstaps " + (data->speedsteps));
		return;
	}

	// Addr
	pb[6] = (addr >> 8) & 0x3F;
	if (addr >= 128) {
		pb[6] += 0b11000000;
	}
	pb[7] = addr & 255;
	pb[8] = speed | ((data->direction == -1) ? 0 : 128);
	pb[9] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7] ^ pb[8];
	send();

}

void CmdReceiverZ21Wlan::sendDCCFun(int addr, LocData* data, unsigned int changedBit) {
	memset(pb, 0, packetBufferSize);
	pb[0] = 0x0A;
	pb[1] = 0x00;
	pb[2] = 0x40;
	pb[3] = 0x00;

	pb[4] = 0xE4;
	pb[5] = 0xF8;
	pb[6] = (addr >> 8) & 0x3F;
	if (addr >= 128) {
		pb[6] += 0b11000000;
	}

	pb[7] = addr & 255;

	pb[8] = bit_is_set01(data->status, changedBit) << 6 | changedBit;


	pb[9] = pb[4] ^ pb[5] ^ pb[6] ^ pb[7] ^ pb[8];
	printPacketBuffer("DCC", pb, pb[0]);
	send();
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
  pb[8] = pb[4] ^ pb[5] ^ pb[6]
                                     ^ pb[7];
  send();
}


CmdReceiverZ21Wlan::~CmdReceiverZ21Wlan() {
}

void CmdReceiverZ21Wlan::resetTimeout() {
	if (timeout == 0) {
		Logger::log(LogLevel::INFO, "Message after Timeout from Z21 received!");
	}
	if (timeout == 1) {
		Logger::log(LogLevel::INFO, "First Command from Z21 received!");
	}
	timeout = millis();
}

void CmdReceiverZ21Wlan::send() {
	udp->beginPacket(*z21Server, localPort);
	udp->write(pb, pb[0]);
	udp->endPacket();
}
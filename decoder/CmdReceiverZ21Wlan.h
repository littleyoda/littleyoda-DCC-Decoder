/*
 * CmdReceiverZ21Wlan.h
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERZ21WLAN_H_
#define CMDRECEIVERZ21WLAN_H_

#include "CmdSenderBase.h"
#include "z21PaketParser.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define OPC_INPUT_REP_CB  0x40  /* control bit, reserved otherwise      */
#define OPC_INPUT_REP_SW  0x20  /* input is switch input, aux otherwise */
#define OPC_INPUT_REP_HI  0x10  /* input is HI, LO otherwise            */
#define OPC_INPUT_REP     0xb2

#define LN_CHECKSUM_SEED  0xFF


class CmdReceiverZ21Wlan: public CmdSenderBase, public z21PaketParser {
public:
	CmdReceiverZ21Wlan(Controller* c, String ip);
	virtual int loop();
	virtual ~CmdReceiverZ21Wlan();
	void requestTurnoutInfo(int addr);
	void requestLocoInfo(int addr);
	void enableBroadcasts();
	virtual void sendSetTurnout(String id, String status);
  virtual void sendSetSensor(uint16_t id, uint8_t status);
private:
	WiFiUDP* udp;
	unsigned int localPort = 21105;
	const int packetBufferSize = 30;
	unsigned char packetBuffer[30];
	unsigned char packetcfg12[30];
	unsigned char packetcfg16[30];
	unsigned long timeout = 0;
	IPAddress* z21Server;
	void doReceive();
	void resetTimeout();

	void sendLanGetSerialNumber();
	void sendCfg12Request();
	void sendCfg16Request();
	void sendFrimwareVersionRequest();
	void sendXGetStatus();
	void sendGetBroadcastFlags();

	void requestRailcom();

	long int lastTime = 0;
	static const int emergencyStopTimeout = 1000;
	static const int cmdSendTime = 100;
	const int firstLoopStatus = -1;
	int loopStatus = firstLoopStatus;
	int subloopstatus = 0;
};

#endif /* CMDRECEIVERZ21WLAN_H_ */

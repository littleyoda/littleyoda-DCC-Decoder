/*
 * CmdReceiverZ21Wlan.h
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERZ21WLAN_H_
#define CMDRECEIVERZ21WLAN_H_

#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

class CmdReceiverZ21Wlan: public CmdReceiverBase, public CmdSenderBase {
public:
	CmdReceiverZ21Wlan(Controller* c, const char* ip);
	virtual int loop();
	virtual ~CmdReceiverZ21Wlan();
	void requestTurnoutInfo(int addr);
	void requestLocoInfo(int addr);
	void enableBroadcasts();
	virtual void sendSetTurnout(String id, String status);

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
	void handleTurnout();
	void handleDCCSpeed(unsigned int locoid);
	void handleFunc(unsigned int locoid);
	void sendLanGetSerialNumber();
	void sendCfg12Request();
	void sendCfg16Request();
	void sendFrimwareVersionRequest();
	void sendXGetStatus();
	void sendGetBroadcastFlags();
	void requestRailcom();
	void handleRailcomdata();
	void printPacketBuffer(int size);
	void handleFirmware();

	void emergencyStop();

	long int lastTime = 0;
	static const int emergencyStopTimeout = 1000;
	const int firstLoopStatus = -4;
	int loopStatus = firstLoopStatus;
	int turnoutOffset = -1;
	int firmwareVersion = 0;
	unsigned char lastZ21Status = 0;
};

#endif /* CMDRECEIVERZ21WLAN_H_ */

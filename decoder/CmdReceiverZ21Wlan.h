/*
 * CmdReceiverZ21Wlan.h
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERZ21WLAN_H_
#define CMDRECEIVERZ21WLAN_H_

#include "CmdSenderBase.h"
#include "Z21Format.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <IStatus.h>


class CmdReceiverZ21Wlan:  public Z21Format  {
public:
	CmdReceiverZ21Wlan(Controller* c, String ip);
	virtual int loop();
	virtual ~CmdReceiverZ21Wlan();
/*	void requestTurnoutInfo(int addr);
	void requestLocoInfo(int addr);
	void enableBroadcasts();*/
	virtual void sendSetTurnout(String id, String status);
    virtual void sendSetSensor(uint16_t id, uint8_t status);
	virtual void sendDCCSpeed(int id, LocData* d);
	virtual void sendDCCFun(int id, LocData* d,  unsigned int changedBit);
	bool udpSetup = false;
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key);
	virtual String createDebugDiagramm(String parent);
private:
	WiFiUDP* udp = nullptr;
	unsigned int localPort = 21105;
//	unsigned char packetcfg12[30];
//	unsigned char packetcfg16[30];
	unsigned long timeout = 1;
	unsigned long timeouts = 0;

	IPAddress* z21Server;
	void doReceive();
	void resetTimeout();

//	void sendLanGetSerialNumber();
//	void sendCfg16Request();
	void sendGetBroadcastFlags();
	virtual void send();


	long int lastTime = 0;
	static const int emergencyStopTimeout = 1000;
	static const int cmdSendTime = 100;
	const int firstLoopStatus = -1;
	int loopStatus = firstLoopStatus;
	int subloopstatus = 0;
};

#endif /* CMDRECEIVERZ21WLAN_H_ */

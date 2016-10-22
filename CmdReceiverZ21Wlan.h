/*
 * CmdReceiverZ21Wlan.h
 *
 *  Created on: 18.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERZ21WLAN_H_
#define CMDRECEIVERZ21WLAN_H_

#include "CmdReceiverBase.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

class CmdReceiverZ21Wlan: public CmdReceiverBase {
public:
	CmdReceiverZ21Wlan(Controller* c, uint8_t ip1, uint8_t ip2, uint8_t ip3,
			uint8_t ip4);
	virtual int loop();
	virtual ~CmdReceiverZ21Wlan();
	void requestTurnoutInfo(int addr);
	void enableBroadcasts();
	void sendSetTurnout(String id, String status);

private:
	WiFiUDP* udp;
	unsigned int localPort = 21105;
	const int packetBufferSize = 30;
	unsigned char packetBuffer[30];
	IPAddress* z21Server;
	void doReceive(int cb);
	void handleTurnout();
	void handleDCCSpeed(unsigned int locoid);
	void handleFunc(unsigned int locoid);

	long int lastTime = 0;
};

#endif /* CMDRECEIVERZ21WLAN_H_ */

/*
 * RocnetOverMQTT.h
 *
 *  Created on: 17.06.2018
 *      Author: sven
 */

#include  <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "PubSubClient.h"
#include "CmdReceiverBase.h"

#ifndef CMDRECEIVERROCNETOVERMQTT_H_
#define CMDRECEIVERROCNETOVERMQTT_H_

class CmdReceiverRocnetOverMQTT: public CmdReceiverBase  {
public:
	static CmdReceiverRocnetOverMQTT* _instance; // Hack for access from PubSubClient
	CmdReceiverRocnetOverMQTT(Controller* c);
	virtual ~CmdReceiverRocnetOverMQTT();
	virtual int loop();
	void parse(String s);

private:
	WiFiClient espClient;
	PubSubClient* client;
	String extractXMLAttribute(String payloads, String attrname);
	String extractString(String payloads, String begin, String end);
	WiFiUDP Udp;
	int discoveryModus = 0;
};

void rocnetovermqttcallback(char* topic, byte* payload, unsigned int length);

#endif /* CMDRECEIVERROCNETOVERMQTT_H_ */

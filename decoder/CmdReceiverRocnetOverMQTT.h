/*
 * RocnetOverMQTT.h
 *
 *  Created on: 17.06.2018
 *      Author: sven
 */


#ifdef ESP8266
	#include <ESP8266WiFi.h>
#elif ESP32
	#include <WiFi.h>
#else
	#error "This Arch is not supported"
#endif

#include <WiFiUdp.h>
#include "PubSubClient.h"
#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include "LocData.h"
#include "LinkedList.h"
#include "INotify.h"
#include "IStatus.h"

#ifndef CMDRECEIVERROCNETOVERMQTT_H_
#define CMDRECEIVERROCNETOVERMQTT_H_

class CmdReceiverRocnetOverMQTT: public CmdReceiverBase, public CmdSenderBase, public IStatus  {
public:
	static CmdReceiverRocnetOverMQTT* _instance; // Hack for access from PubSubClient
	CmdReceiverRocnetOverMQTT(Controller* c);
	virtual ~CmdReceiverRocnetOverMQTT();
	virtual int loop();
	void parse(String s);
	virtual void sendSetTurnout(String id, String status)  override { /* TODO */ };
	virtual void sendSetSensor(uint16_t id, uint8_t status)  override { /* TODO */  };
	virtual void sendDCCSpeed(int id, LocData* d)  override;
	virtual void sendDCCFun(int id, LocData* d,  unsigned int changedBit)  override;
	virtual void setRequestList(LinkedList<INotify::requestInfo*>* list)  override {requestList = list;};
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key)  override;
	void setMQTT(String addr, int port);
private:
	WiFiClient espClient;
	PubSubClient* client;
	String extractXMLAttribute(String payloads, String attrname);
	String extractString(String payloads, String begin, String end);
	String host = "";
	int port = 0;
	WiFiUDP Udp;
	int trying = 0;
	int discoveryModus = 0;
};

void rocnetovermqttcallback(char* topic, byte* payload, unsigned int length);

#endif /* CMDRECEIVERROCNETOVERMQTT_H_ */

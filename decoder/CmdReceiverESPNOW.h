/*
 * CmdReceiverESPNOW.h
 *
 *  Created on: 01.01.2018
 *      Author: sven
 */

#ifndef CMDRECEIVERESPNOW_H_
#define CMDRECEIVERESPNOW_H_

#include "Z21Format.h"
#include "Controller.h"
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include "Logger.h"
extern "C" {
#include <espnow.h>
#include "user_interface.h"
}
#elif ESP32
	#include <Arduino.h>
	#include <esp_now.h>
	#include <WiFi.h>
#endif
#include <Crypto.h>
#include <AES.h>

#define espnowEncrypt 

class CmdReceiverESPNOW : public Z21Format  {
public:
	CmdReceiverESPNOW(Controller* c, String rolle,String key, int kanal);
	virtual ~CmdReceiverESPNOW();
	virtual int loop();

private:
	static CmdReceiverESPNOW* instance;
	  enum messagetyps : uint16_t {
		  internal = 0,
		  z21FromMaster = 1,
		  z21ToMaster = 2
	  } ;
	  enum internalSubs : uint16_t {
		  echoRequest = 0,
		  echoReply = 1
	  } ;
	struct msg {
	  messagetyps typ;
	  uint16_t subtyp;
	  uint16_t hops;
	  uint32_t id;
	  uint8_t msg[22];
	} __attribute__ ((packed));
	byte key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

	enum rollen {
	  master,
	  repeater,
	  receiver,
	  unknown
	} rolle;
	AES128 cipher;
	uint8_t broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	LinkedList<msg*> sendQuery= LinkedList<msg*>();
	LinkedList<msg*> receivedQuery= LinkedList<msg*>();
	static int msgsize;
	#ifdef ESP32
	static void cb_MsgReceived(const uint8_t *mac_addr, const uint8_t *data, int len);
	#endif
	#ifdef ESP8266
	static void cb_MsgReceived(u8 *mac_addr, u8 *data, u8 len);
	#endif
	virtual void send();
	unsigned long last = 0;
	int loopStatus = 0;
	String rolename;
	int msgIdsSize = 10;
	uint32_t msgIds[10];
	int msgIdsIdx = 0;
	void sendEchoReply();
	void sendEchoRequest();
	void addMsgId(uint32_t id);
	bool existsMsgId(uint32_t id);
	uint32_t myid;
	void handleEchoReply(msg* m);
	virtual void getInternalStatus(IInternalStatusCallback* cb, String key);
	String status;
	String oldstatus;
};

#endif /* CMDRECEIVERESPNOW_H_ */

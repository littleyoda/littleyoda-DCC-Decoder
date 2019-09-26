/*
 * CmdReceiverESPNOW.h
 *
 *  Created on: 01.01.2018
 *      Author: sven
 */

#ifndef CMDRECEIVERESPNOW_H_
#define CMDRECEIVERESPNOW_H_

#ifdef ESP32

#include "z21PaketParser.h"
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


class CmdReceiverESPNOW : public z21PaketParser  {
public:
	CmdReceiverESPNOW(Controller* c, String rolle);
	virtual ~CmdReceiverESPNOW();
	virtual int loop();

private:
	static CmdReceiverESPNOW* instance;
	  enum messagetyps : uint16_t {
		  internal = 0,
		  z21cmd
	  } ;
	struct msg {
	  messagetyps typ;
	  uint16_t hops;
	  uint32_t id;
	  uint8_t msg[22];
	};

	enum rollen {
	  master,
	  repeater,
	  receiver,
	  unknown
	} rolle;

	uint8_t broadcastMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t key[16] = {
									0x32, 0xA3, 0x32, 0xA8, 0xC8, 
									0xF0, 0x32, 0xA3, 0x32, 0xA8, 
									0xC8, 0X77, 0x98, 0xB7, 0x66,
									0x88
								};
	static bool readyForSend;
	LinkedList<msg*> sendQuery= LinkedList<msg*>();
	LinkedList<msg*> receivedQuery= LinkedList<msg*>();
	static int msgsize;

	static void cb_MsgReceived(const uint8_t *mac_addr, const uint8_t *data, int len);
	static void cb_MsgSend(const uint8_t *mac_addr, esp_now_send_status_t status);

	unsigned long last = 0;
	String rolename;
};

#endif /* CMDRECEIVERESPNOW_H_ */
#endif
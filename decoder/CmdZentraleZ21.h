/**
 *  Simuliert eine Z21
 *      Author: sven
 */

#ifndef CmdZentraleZ21_H_
#define CmdZentraleZ21_H_

#include <WiFiClient.h>
#include <WiFiUdp.h>
#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include "LinkedList.h"
#include "LocData.h"

struct ClientsStruct {
		IPAddress addr;
		uint16_t port;
} __attribute__ ((packed));

class CmdZentraleZ21: public CmdReceiverBase, public CmdSenderBase {
public:
	CmdZentraleZ21(Controller* c);
	virtual int loop();
	virtual ~CmdZentraleZ21();
	virtual void sendSetTurnout(String id, String status);
	virtual void sendSetSensor(uint16_t id, uint8_t status);
	virtual void sendDCCSpeed(int id, LocData* d);

private:
	void handleLocoMode();
	void handleSetLoco(int locoid);
	void handleGetSerial();
	void handleGetVersion();
	void handleGetStatus();
	void handleBIB();
	void handleTurnInfoRequest(int id);
	int handleSetTurnInfoRequest();

	void sendFirmware();
	void sendLocoInfoToClient(int locid);
	void sendHwinfo();
	void sendStatusChanged();


	WiFiUDP* udp;
	unsigned int localPort = 21105;
	const int packetBufferSize = 30;
	unsigned char pb[30];
	unsigned long timeout = 0;
	void doReceive();
	void resetTimeout();
	void handleBroadcast();

	void printPacketBuffer(int size);

	void emergencyStop();
	void handleSetLocoFunc(unsigned int locoid);

	long int lastTime = 0;
	static const int emergencyStopTimeout = 4200;
	LinkedList<ClientsStruct*>* clients;

};

#endif /* CmdZentraleZ21_H_ */

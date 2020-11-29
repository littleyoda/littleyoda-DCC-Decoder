/**
 *  Simuliert eine Z21
 *      Author: sven
 */

#ifndef CmdZentraleZ21_H_
#define CmdZentraleZ21_H_

#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <INotify.h>
#include <LinkedList.h>
#include "Z21Format.h"
#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include "LinkedList.h"
#include "LocData.h"


struct Z21Clients {
		IPAddress ip;
		uint16_t port;
		uint32_t flags; 
		LinkedList<int> ids = LinkedList<int>();
		uint16_t broadcastIdx = 0;
};


class CmdZentraleZ21: public Z21Format, public INotify {
public:
	CmdZentraleZ21(Controller* c);
	virtual int loop();
	virtual ~CmdZentraleZ21();
	virtual void sendSetTurnout(String id, String status);
	virtual void sendSetSensor(uint16_t id, uint8_t status);
	virtual void sendDCCSpeed(int addr, LocData* data);
	virtual void sendDCCFun(int id, LocData* d,  unsigned int changedBit);
	String getInternalStatus(String key);
	virtual String createDebugDiagramm(String parent);
protected:
	virtual void adjustBroadcast(int addr);


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
//	void sendLocoInfoToClient(int locid);
	void sendHwinfo();
	void sendStatusChanged();

	virtual void send();
	Controller* cnt;

	WiFiUDP* udp;
	bool udpSetup = false;
	unsigned int localPort = 21105;
	unsigned long timeout = 0;
	unsigned long nextSend = 0;
	void doReceive();
	void resetTimeout();
	void handleBroadcast();

	void printPacketBuffer(int size);

	void emergencyStop();
//	void handleSetLocoFunc(unsigned int locoid);

	long int lastBroadcastTime = 0;
	static const int emergencyStopTimeout = 4200;

	LinkedList<Z21Clients*> clients = LinkedList<Z21Clients*>();
	Z21Clients* getClient(IPAddress addr, uint16_t port);
	uint16_t broadcastClientIdx = 0;
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	IPAddress currentDestIP;
	uint16_t currentDestPort =  0;

};

#endif /* CmdZentraleZ21_H_ */

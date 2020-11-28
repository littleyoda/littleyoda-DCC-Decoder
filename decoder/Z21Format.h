/*
 * Z21FORMAT.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef Z21FORMAT_H_
#define Z21FORMAT_H_

#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include "IStatus.h"

class Controller;

class Z21Format : public CmdReceiverBase, public CmdSenderBase, public IStatus {
public:
	Z21Format(Controller* c);
	virtual ~Z21Format();
	bool parseClient2Server(unsigned char packetBuffer[], int cb);
	bool parseServer2Client(unsigned char packetBuffer[], int cb);

	void sendFirmware();
	void sendHwinfo();
	void handleGetStatus();
	void handleGetSerial();
	void handleGetVersion();
	void sendStatusChanged();
	void handleTurnInfoRequest(int id);
	void handleLocoMode();
	void sendXGetStatus();
	const int packetBufferSize = 30;
	unsigned char pb[30];
	int handleSetTurnInfoRequest();
	void sendLocoInfoToClient(int addr);
	void handleSetLocoFunc(unsigned int locoid);
	void handleSetLoco(int locoid);
	void DCCSpeed(int addr, int speed, int direction, int SpeedSteps, int source);
	void sendCfg16Request();
	void sendCfg12Request();
	void sendFrimwareVersionRequest();
	void requestTurnoutInfo(int addr);
	void enableBroadcasts();
	void sendSetTurnout(String id, String status);
	void requestLocoInfo(int addr);
	virtual void setName(String s) {};
	virtual String getName();
	virtual String getInternalStatus(String key);

protected:
	bool debugEnabled = true;
	virtual void send();
	unsigned char lastZ21Status = 0;
	void printPacketBuffer(String msg, unsigned char packetBuffer[], int size);
	void emergencyStop();
	void handleTurnout(unsigned char packetBuffer[]);
	void handleDCCSpeed(unsigned int locoid, unsigned char packetBuffer[]);
	void handleFunc(unsigned int locoid, unsigned char packetBuffer[]);
	void handleRailcomdata(unsigned char packetBuffer[]);
	void handleFirmware(unsigned char packetBuffer[]);
	int turnoutOffset = -1;
	int firmwareVersion = 0;
	Controller* controller;
	virtual void adjustBroadcast(int addr);
	void debugMsg(String s);

};

#endif /* Z21FORMAT_H_ */

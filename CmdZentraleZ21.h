/*
 *      Author: sven
 */

#ifndef CmdZentraleZ21_H_
#define CmdZentraleZ21_H_

#include "CmdReceiverBase.h"
#include "CmdSenderBase.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

class CmdZentraleZ21: public CmdReceiverBase {
public:
	CmdZentraleZ21(Controller* c);
	virtual int loop();
	virtual ~CmdZentraleZ21();

private:
	void handleLocoMode();
	void sendLocoInfoToClient(int locid);
	void handleSetLoco(int locoid);
	void handleGetSerial();
	void handleGetVersion();
	void handleGetStatus();
	void sendFirmware();
	void sendHwinfo();
	void handleBIB();
	void sendStatusChanged();
	void handleTurnInfoRequest(int id);
	int handleSetTurnInfoRequest();


	WiFiUDP* udp;
	unsigned int localPort = 21105;
	const int packetBufferSize = 30;
	unsigned char pb[30];
	unsigned long timeout = 0;
	void doReceive(int cb);
	void resetTimeout();

	void printPacketBuffer(int size);

	void emergencyStop();
	void handleSetLocoFunc(unsigned int locoid);

	long int lastTime = 0;
	static const int emergencyStopTimeout = 4200;
};

#endif /* CmdZentraleZ21_H_ */

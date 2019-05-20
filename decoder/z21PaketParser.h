/*
 * z21PaketParser.h
 *
 *  Created on: 01.01.2018
 *      Author: sven
 */

#ifndef Z21PAKETPARSER_H_
#define Z21PAKETPARSER_H_

#include <Arduino.h>
#include "Logger.h"
#include "CmdSenderBase.h"
#include "Controller.h"



class z21PaketParser : public CmdReceiverBase {
public:
	z21PaketParser(Controller* c);
	virtual ~z21PaketParser();
	void parser(unsigned char packetBuffer[], int cb);

protected:
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


};

#endif /* Z21PAKETPARSER_H_ */

/*
 * CmdReceiverDCC.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERDCC_H_
#define CMDRECEIVERDCC_H_

#include "CmdReceiverBase.h"
#include "NmraDcc2.h"
#include "Controller.h"

class CmdReceiverDCC: public CmdReceiverBase {
private:
	NmraDcc Dcc;
	DCC_MSG Packet;

public:
	static CmdReceiverDCC* _instance; // Hack for access from NmraDCC
	CmdReceiverDCC(Controller* c, uint8_t ExtIntNum, uint8_t ExtIntPinNum);
	virtual ~CmdReceiverDCC();
	virtual void loop();
	virtual void handleTurnOut(uint16_t Addr, uint8_t Direction);
	virtual void handleDccSpeed(uint16_t Addr, uint8_t Speed, DCC_DIRECTION Dir,
			DCC_SPEED_STEPS SpeedSteps);
	virtual void handleDccFun(uint16_t Addr, FN_GROUP FuncGrp,
			uint8_t FuncState);
//	static void handleTurnOut(uint16_t Addr, uint8_t Direction, uint8_t OutputPower);
};

#endif /* CMDRECEIVERDCC_H_ */

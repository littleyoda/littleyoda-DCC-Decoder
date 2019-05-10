/*
 * CmdReceiverDCC.cpp
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#include "CmdReceiverDCC.h"

#include "NmraDcc2.h"
#include "Consts.h"

// Will be called by NMRADCC
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp,
		uint8_t FuncState) {
	CmdReceiverDCC::_instance->handleDccFun(Addr, FuncGrp, FuncState);
}

void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed,
		DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
	CmdReceiverDCC::_instance->handleDccSpeed(Addr, Speed, Dir, SpeedSteps);
}

// DCC Turnout Packet received
void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction,
		uint8_t OutputPower) {
	if (OutputPower == 1) {
		CmdReceiverDCC::_instance->handleTurnOut(Addr, Direction);
	}
}

CmdReceiverDCC::CmdReceiverDCC(Controller* c, uint8_t ExtIntNum,
		uint8_t ExtIntPinNum) :
		CmdReceiverBase(c) {
	Logger::getInstance()->addToLog("Starting DCC Receiver (PIN: " + String(ExtIntNum) + ")...");
	CmdReceiverDCC::_instance = this;
	Dcc.pin(ExtIntNum, ExtIntPinNum, false);
	Dcc.init( MAN_ID_DIY, 10,
			CV29_ACCESSORY_DECODER | FLAGS_OUTPUT_ADDRESS_MODE, 0);
}

CmdReceiverDCC::~CmdReceiverDCC() {
}

int CmdReceiverDCC::loop() {
	Dcc.process();
	return 0;
}

CmdReceiverDCC *CmdReceiverDCC::_instance = NULL;

void CmdReceiverDCC::handleTurnOut(uint16_t Addr, uint8_t Direction) {
	controller->notifyTurnout(Addr, Direction, 0);
}

void CmdReceiverDCC::handleDccSpeed(uint16_t Addr, uint8_t Speed,
		DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
	controller->notifyDCCSpeed(Addr, Speed, (Dir == 1) ? Dir : Consts::SPEED_REVERSE, SpeedSteps, 0);
}

void CmdReceiverDCC::handleDccFun(uint16_t Addr, FN_GROUP FuncGrp,
		uint8_t FuncState) {
	int startbit = -1;
	int stopbit = -1;
	unsigned long value = 0;
	switch (FuncGrp) {
#ifdef NMRA_DCC_ENABLE_14_SPEED_STEP_MODE
	case FN_0:
	value = (FuncState & FN_BIT_00) ? 1 : 0;
	break;
#endif
	case FN_0_4:
		startbit = 0;
		stopbit = 4;
		value = ((FuncState & FN_BIT_00) ? 1 : 0)
				+ ((FuncState & FN_BIT_01) ? 2 : 0)
				+ ((FuncState & FN_BIT_02) ? 4 : 0)
				+ ((FuncState & FN_BIT_03) ? 8 : 0)
				+ ((FuncState & FN_BIT_04) ? 16 : 0);
		break;

	case FN_5_8:
		startbit = 5;
		stopbit = 8;
		value = ((FuncState & FN_BIT_05) ? 1 : 0)
				+ ((FuncState & FN_BIT_06) ? 2 : 0)
				+ ((FuncState & FN_BIT_07) ? 4 : 0)
				+ ((FuncState & FN_BIT_08) ? 8 : 0);
		value = value << 5;
		break;

	case FN_9_12:
		startbit = 9;
		stopbit = 12;
		value = ((FuncState & FN_BIT_09) ? 1 : 0)
				+ ((FuncState & FN_BIT_10) ? 2 : 0)
				+ ((FuncState & FN_BIT_11) ? 4 : 0)
				+ ((FuncState & FN_BIT_12) ? 8 : 0);
		value = value << 9;
		break;

	case FN_13_20:
		startbit = 13;
		stopbit = 20;
		value = ((FuncState & FN_BIT_13) ? 1 : 0)
				+ ((FuncState & FN_BIT_14) ? 2 : 0)
				+ ((FuncState & FN_BIT_15) ? 4 : 0)
				+ ((FuncState & FN_BIT_16) ? 8 : 0)
				+ ((FuncState & FN_BIT_17) ? 16 : 0)
				+ ((FuncState & FN_BIT_18) ? 32 : 0)
				+ ((FuncState & FN_BIT_19) ? 64 : 0)
				+ ((FuncState & FN_BIT_20) ? 128 : 0);
		value = value << 13;
		break;

	case FN_21_28:
		startbit = 21;
		stopbit = 28;
		value = ((FuncState & FN_BIT_21) ? 1 : 0)
				+ ((FuncState & FN_BIT_22) ? 2 : 0)
				+ ((FuncState & FN_BIT_23) ? 4 : 0)
				+ ((FuncState & FN_BIT_24) ? 8 : 0)
				+ ((FuncState & FN_BIT_25) ? 16 : 0)
				+ ((FuncState & FN_BIT_26) ? 32 : 0)
				+ ((FuncState & FN_BIT_27) ? 64 : 0)
				+ ((FuncState & FN_BIT_28) ? 128 : 0);
		value = value << 21;
		break;
	}
	controller->notifyDCCFun(Addr, startbit, stopbit, value, 0);
}

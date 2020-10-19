/*
 * CmdSenderBase.h

 *
 *  Created on: 13.12.2016
 *      Author: sven
 */
#ifndef CMDSENDERBASE_H_
#define CMDSENDERBASE_H_

#include <WString.h>
#include <LinkedList.h>

#include "INotify.h"
#include "LocData.h"

class CmdSenderBase {
public:
	CmdSenderBase() {};
	virtual ~CmdSenderBase() {};
	virtual void sendSetTurnout(String id, String status) {};
	virtual void sendSetSensor(uint16_t id, uint8_t status) {};
	virtual void sendDCCSpeed(int id, LocData* d) {};
	virtual void sendDCCFun(int id, LocData* d,  unsigned int changedBit) {};
	virtual void setRequestList(LinkedList<INotify::requestInfo*>* list) {requestList = list;};

protected:
	LinkedList<INotify::requestInfo*>* requestList = 0;
};

#endif /* CMDSENDERBASE_H_ */

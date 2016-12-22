/*
 * CmdSenderBase.h

 *
 *  Created on: 13.12.2016
 *      Author: sven
 */
#include <WString.h>
#include "ActionBase.h"
#include <LinkedList.h>
#ifndef CMDSENDERBASE_H_
#define CMDSENDERBASE_H_

class CmdSenderBase {
public:
	CmdSenderBase();
	virtual ~CmdSenderBase();
	virtual void sendSetTurnout(String id, String status);
	virtual void setRequestList(LinkedList<ActionBase::requestInfo*>* list);

protected:
	LinkedList<ActionBase::requestInfo*>* requestList = 0;
};

#endif /* CMDSENDERBASE_H_ */

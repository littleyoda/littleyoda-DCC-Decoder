/*
 * CmdSenderBase.h

 *
 *  Created on: 13.12.2016
 *      Author: sven
 */
#include <WString.h>
#include <LinkedList.h>

#include "INotify.h"
#ifndef CMDSENDERBASE_H_
#define CMDSENDERBASE_H_

class CmdSenderBase {
public:
	CmdSenderBase();
	virtual ~CmdSenderBase();
	virtual void sendSetTurnout(String id, String status);
	virtual void setRequestList(LinkedList<INotify::requestInfo*>* list);

protected:
	LinkedList<INotify::requestInfo*>* requestList = 0;
};

#endif /* CMDSENDERBASE_H_ */

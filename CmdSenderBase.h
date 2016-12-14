/*
 * CmdSenderBase.h

 *
 *  Created on: 13.12.2016
 *      Author: sven
 */
#include <WString.h>
#ifndef CMDSENDERBASE_H_
#define CMDSENDERBASE_H_

class CmdSenderBase {
public:
	CmdSenderBase();
	virtual ~CmdSenderBase();
	virtual void sendSetTurnout(String id, String status);

};

#endif /* CMDSENDERBASE_H_ */

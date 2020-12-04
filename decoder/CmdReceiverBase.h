/*
 * CmdReceiverBase.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERBASE_H_
#define CMDRECEIVERBASE_H_

#include "ILoop.h"

class Controller;
// forward declaration

class CmdReceiverBase : public ILoop {
public:
	CmdReceiverBase(Controller* c);
	virtual ~CmdReceiverBase();

protected:
	Controller* controller;
};

#endif /* CMDRECEIVERBASE_H_ */

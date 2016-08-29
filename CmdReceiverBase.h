/*
 * CmdReceiverBase.h
 *
 *  Created on: 17.08.2016
 *      Author: sven
 */

#ifndef CMDRECEIVERBASE_H_
#define CMDRECEIVERBASE_H_

class Controller;
// forward declaration

class CmdReceiverBase {
public:
	CmdReceiverBase(Controller* c);
	virtual ~CmdReceiverBase();
	virtual void loop() = 0;

protected:
	Controller* controller;
};

#endif /* CMDRECEIVERBASE_H_ */

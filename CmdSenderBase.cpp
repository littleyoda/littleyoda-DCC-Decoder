/*
 * CmdSenderBase.cpp
 *
 *  Created on: 13.12.2016
 *      Author: sven
 */

#include "CmdSenderBase.h"

CmdSenderBase::CmdSenderBase() {
}

CmdSenderBase::~CmdSenderBase() {
}

void CmdSenderBase::sendSetTurnout(String id, String status) {
}

void CmdSenderBase::setRequestList(LinkedList<ActionBase::requestInfo*>* list) {
	requestList = list;
}

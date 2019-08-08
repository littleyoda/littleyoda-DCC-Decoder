/*
 * ISTATUS.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "IStatus.h"

IStatus::IStatus() {
	id = NULL;
}

IStatus::~IStatus() {
	// TODO Auto-generated destructor stub
}

String IStatus::getInternalStatus(String key) {
	return "";
}

void IStatus::setName(String s) {
	id = new String(s);
}

String IStatus::getName() {
	if (id == NULL) {
		return "";
	}
	return "" + (*id);
}

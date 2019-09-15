/*
 * INTERNALSTATUSASSTRING.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InternalStatusAsString.h"

InternalStatusAsString::InternalStatusAsString() {
}

InternalStatusAsString::~InternalStatusAsString() {
}

void InternalStatusAsString::reset() {
	value = "";
}

String InternalStatusAsString::get() {
	return value;
}

void InternalStatusAsString::send(String modul, String key, String v) {
	value = v;
}

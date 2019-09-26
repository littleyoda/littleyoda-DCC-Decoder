/*
 * INAMED.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Utils.h"
#include "INamed.h"

INamed::INamed() {
	id = new String(Utils::getRnd());
	modulname = "Unbekannt";
	cfgDesc = "";
}

INamed::~INamed() {
}

void INamed::setName(String s) {
	id = new String(s);
}

String INamed::getName() {
	if (id == NULL) {
		return "";
	}
	return "" + (*id);
}

void INamed::setModulName(String name) {
	modulname = name;
}
void INamed::setConfigDescription(String desc) {
	cfgDesc = desc;
}

String INamed::createDebugDiagramm(String parent) {
	return "";
}

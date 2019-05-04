/*
 * ISettings.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ISettings.h"

ISettings::ISettings() {
	id = NULL;
}

ISettings::~ISettings() {
}

void ISettings::setSettings(String key, String value) {
}

String ISettings::getHTMLCfg(String urlprefix) { return ""; }

String ISettings::getHTMLController(String urlprefix) {
	return "";
}

void ISettings::setName(String s) {
	id = new String(s);
}

String ISettings::getName() {
	if (id == NULL) {
		return "";
	}
	return "" + (*id);
}

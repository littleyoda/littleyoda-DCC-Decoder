/*
 * ISettings.cpp
 *
 *  Created on: 19.07.2017
 *      Author: sven
 */

#include "ISettings.h"
#include "Controller.h"

ISettings::ISettings() : IStatus() {
}

ISettings::~ISettings() {
}

void ISettings::setSettings(String key, String value) {
}

String ISettings::getHTMLCfg(String urlprefix) { return ""; }

String ISettings::getHTMLController(String urlprefix) {
	return "";
}


void ISettings::getHTMLConfig(String urlprefix, Controller* c) {
	// Abwärtskompatibilität
	String s = getHTMLCfg(urlprefix);
	if (s != NULL && s != "") {
		c->sendContent(s);
	}
}


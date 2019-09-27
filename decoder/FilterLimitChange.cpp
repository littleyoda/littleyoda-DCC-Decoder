/*
 * FILTERLIMITCHANGE.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "FilterLimitChange.h"

FilterLimitChange::FilterLimitChange(int r) {
	value = 0;
	current = 0;
	name = "";
	rate = fabs(r) / 10.0f;
	setModulName("Limit Change");
	setConfigDescription("Rate: " + String(abs(r)) + "/sec");
}

void FilterLimitChange::setSettings(String key, String val) {
	if (key.equals("sd")) {
		value = val.toInt();
		name = key;
	} else {
		send(key, val);
	}
}

FilterLimitChange::~FilterLimitChange() {
}

int FilterLimitChange::loop() {
	if (abs(current-value) < 0.2) {
		return 100;
	}
	if (current < value) {
		current += rate;
		if (current > value) {
			current = value;
		}
	} else if (current > value) {
		current -= rate;
		if (current < value) {
			current = value;
		}
	}
	send(name, String((int)(current)));
	return 100;
}

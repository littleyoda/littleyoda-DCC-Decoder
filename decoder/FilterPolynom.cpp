/*
 * FILTERPOLYNOM.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "FilterPolynom.h"
#include "Logger.h"

FilterPolynom::FilterPolynom(String k) {
	key = k;
	setModulName("Limit Change");
	setConfigDescription("");
}


void FilterPolynom::addPolynom(String typ, float a, float xdiff, float y) {
	if (typ == "poly") {
		polyValues* pv = new polyValues();
		pv->a = a;
		pv->xdiff = xdiff;
		pv->y = y;
		polynom.add(pv);
	} else {
		Logger::log(LogLevel::ERROR, "Unbekannter Typ: " + typ);
	}
}

FilterPolynom::~FilterPolynom() {
	// TODO Auto-generated destructor stub
}

int FilterPolynom::calc(int x) {
		float out = 0.0;

		polyValues* pv;
		for(int i = 0; i < polynom.size(); i++){
			pv = polynom.get(i);
			out = out +
			    pv->a * pow((x - pv->xdiff), pv->y);
		}
		return (int) std::round(out);
}

void FilterPolynom::setSettings(String k, String value) {
	if (key.equals("*") || key.equalsIgnoreCase(k)) {
		float x = value.toInt();
		int out = calc(x);
		send(k, String(out));
	} else {
		send(k, value);
	}
}

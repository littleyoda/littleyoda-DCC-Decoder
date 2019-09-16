/*
 * INTERNALSTATUSASJSON.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InternalStatusAsJson.h"

InternalStatusAsJson::InternalStatusAsJson() {
	// TODO Auto-generated constructor stub

}

InternalStatusAsJson::~InternalStatusAsJson() {
	// TODO Auto-generated destructor stub
}

void InternalStatusAsJson::reset() {
	delete(doc);
	doc = new DynamicJsonDocument(2048);
	(*doc)["Hallo"] = "Hallo";
}

void InternalStatusAsJson::send(String modul, String key, String value) {
	if (!doc->containsKey(modul)) {
		doc->createNestedObject(modul);
	}
	JsonObject data = doc->getMember(modul);
	data[key] = value;
}

String InternalStatusAsJson::get() {
	String output;
  	serializeJsonPretty(*doc, output);
	return output;
}

void InternalStatusAsJson::print() {
	serializeJsonPretty(*doc, Serial);
}

/*
 * INTERNALSTATUSASJSON.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InternalStatusAsJson.h"

InternalStatusAsJson::InternalStatusAsJson() {
	doc = NULL;
}

InternalStatusAsJson::~InternalStatusAsJson() {
}

void InternalStatusAsJson::reset() {
	if (doc != NULL) {
		delete(doc);
	}
	if (memoryCritical) {
		memoryCritical = false;
		if  (ESP.getFreeHeap() > 4000) {
			bufferSize += 2024;
		} else if  (ESP.getFreeHeap() > 2000) {
			bufferSize += 1024;
		}
	}
	doc = new DynamicJsonDocument(bufferSize);
	(*doc)["version"] = "1";
}

void InternalStatusAsJson::send(String modul, String key, String value) {
	if (!doc->containsKey(modul)) {
		doc->createNestedObject(modul);
	}
	JsonObject data = doc->getMember(modul);
	data[key] = value;
	if (doc->memoryUsage() > bufferSize - 100) {
		memoryCritical = true;
	}
}

String InternalStatusAsJson::get() {
	String output;
  	serializeJsonPretty(*doc, output);
	return output;
}

void InternalStatusAsJson::print() {
	serializeJsonPretty(*doc, Serial);
}

/*
 * DoubleBootDetection.cpp
 *
 *  Created on: 15.06.2017
 *      Author: sven
 */

#include "DoubleBootDetection.h"
#include <FS.h>

DoubleBootDetection::DoubleBootDetection(Controller* c) {
	controll = c;
}

DoubleBootDetection::~DoubleBootDetection() {
}


int DoubleBootDetection::loop() {
	if (status == 0) {
	//	Serial.println("Check for Existing file");
		boolean e = SPIFFS.exists(filename);
		Serial.println("Exists: " + String(e));
		if (e) {
			// Double Boot Remove File
			SPIFFS.remove(filename);
			status = -1;
			controll->enableAPModus();
		} else {
			// Create File
			File f = SPIFFS.open(filename, "w");
			if (!f) {
			    Serial.println("file open failed");
			}
			f.close();
			status = 1;
		}
	} else if (status == 1 && millis() > 10000) {
//		Serial.println("Removing File");
		SPIFFS.remove(filename);
		status = -1;
	}
	return 2001;
}

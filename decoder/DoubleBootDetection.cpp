/*
 * DoubleBootDetection.cpp
 *
 *  Created on: 15.06.2017
 *      Author: sven
 */

#include "DoubleBootDetection.h"
#include <FS.h>
#ifdef ESP32
	#include "SPIFFS.h"
#endif
#include "Logger.h"

DoubleBootDetection::DoubleBootDetection(Controller* c) {
	controll = c;
}

DoubleBootDetection::~DoubleBootDetection() {
}


int DoubleBootDetection::loop() {
	if (SPIFFS.exists(blockfilename)) {
		return 10000;
	}
	if (status == 0) {
		boolean e = SPIFFS.exists(filename);
		Logger::log(LogLevel::TRACE, "DoubleBootDetection: " + String(e));
		if (e) {
			// Double Boot Remove File
			SPIFFS.remove(filename);
			status = -1;
			controll->enableAPModus();
		} else {
			// Create File
			File f = SPIFFS.open(filename, "w");
			if (!f) {
				Logger::log(LogLevel::ERROR, "Konnte Datei nicht anlegen: " + String(filename));
			}
			f.close();
			status = 1;
		}
	} else if (status == 1 && millis() > 10000) {
		SPIFFS.remove(filename);
		status = -1;
	}
	return 2001;
}

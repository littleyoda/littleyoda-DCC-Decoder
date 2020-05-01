/*
 * TRANSFER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Transfer.h"
#include "Controller.h"
#include "Utils.h"

Transfer::Transfer(Controller* c) {
	status = idle;
	buffer = "";
}

bool Transfer::key(char c) {
	switch (status) {
		case idle:
			if (c == 13) {
				String fileName = Utils::getSubstring(buffer, ' ', 2);
				fileSize = Utils::getSubstring(buffer, ' ', 1).toInt();
				// Serial.println("Command: " + Utils::getSubstring(buffer, ' ', 0));
				// Serial.println("Size: " + String(fileSize));
				// Serial.println("Path: " + fileName);
				if (fileSize < 0 || fileSize > 1000000) {
					Serial.println("STATUS: Filesize invalid");
					return false;
				}
				if (fileName.length() < 1 || fileName.length() > 50) {
					Serial.println("STATUS: Filename invalid");
					return false;
				}
				file = SPIFFS.open("/" + fileName, "w"); 
    			if (!file) {
      				Serial.println("STATUS: Fehler beim schreiben der Datei");
					return false;
      			}
				currentSize = 0;
				fileSize = Utils::getSubstring(buffer, ' ', 1).toInt();
				status = fileput;
				buffer = "";
			} else {
				buffer += c;
			}
			return true;
		case fileput:
			return handleFileput(c);
	}
	return false;
}	

bool Transfer::handleFileput(char c) {
	// Ignore Linebreacks
	if (c == '\n' || c == '\r') {
		return true;
	}
	if (c == 61 || b64.indexOf(c) != -1) {
		buffer += c;
	} else {
		Serial.println("Non Valid Character" + String(int(c)));
		Serial.println("Size " + String(currentSize) + "/" + String(fileSize));
	}
	if (buffer.length() == 4) {
		char c1 = b64.indexOf(buffer[0]) << 2 | (b64.indexOf(buffer[1]) >> 4);
		char c2 = (b64.indexOf(buffer[1]) & 15) << 4 | (b64.indexOf(buffer[2]) >> 2);
		char c3 = (b64.indexOf(buffer[2]) & 3) << 6 | (b64.indexOf(buffer[3]));
		if (buffer[2] == '=') {
			file.print(c1);
			currentSize++;
		} else if (buffer[3] == '=') {
			file.print(c1);
			file.print(c2);
			currentSize += 2;
		} else {
			file.print(c1);
			file.print(c2);
			file.print(c3);
			currentSize += 3;
		}
		if (currentSize == fileSize) {
			file.close();
			Serial.println("STATUS: FINISH");
			status = idle;
			return false;
		}
		if (currentSize > fileSize) {
			file.close();
			Serial.println("STATUS: FAILED");
			status = idle;
			return false;
		}
		buffer = "";
	}
	return true;
}

Transfer::~Transfer() {
	// TODO Auto-generated destructor stub
}

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
	reset();
}

void Transfer::loop() {
	if (transfermode && Utils::timeDiff(lasttransfer, 3000)) {
		Serial.println("SEGMENT FAIL " + String(currentSegment) + " (" + String(buffer.length())+ ")");
		lasttransfer = millis();
		buffer = "";
	}
}
void Transfer::reset() {
		status = idle;
		buffer = "";
		currentSize = 0;
		fileSize = 0;
		b64Size = 0;
        currentSegment = 0;
}

void Transfer::abort(String s ) {
		file.close();
		transfermode = false;
		Serial.println("STATUS: FAILED (" + s + ")");
		reset();
}

bool Transfer::key(char c) {
	lasttransfer = millis();
	switch (status) {
		case idle:
			if (c == 13) {
				if (buffer.isEmpty()) {
					return true;
				}
				fileSize = Utils::getSubstring(buffer, ' ', 1).toInt();
				String fileName = Utils::getSubstring(buffer, ' ', 2);
 				 Serial.println(">" + buffer + "<");
				 Serial.println("Command: " + Utils::getSubstring(buffer, ' ', 0));
				 Serial.println("Size: " + String(fileSize));
				 Serial.println("Path: " + fileName);
				if (!Utils::getSubstring(buffer, ' ', 0).equals("PUT")) {
					abort("Method unknown (" + Utils::getSubstring(buffer, ' ', 0) + ")");
					return false;
				}
				if (fileSize < 0 || fileSize > 1000000) {
					abort("Filesize invalid");
					return false;
				}
				if (fileName.length() < 1 || fileName.length() > 50) {
					abort("Filename invalid");
					return false;
				}
				file = SPIFFS.open("/" + fileName, "w"); 
    			if (!file) {
  					abort("Error while Writing File");
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
			if (c == '\n' || c == '\r') {
				return true;
			}
			if (c == 61 || b64.indexOf(c) != -1) {
				buffer += c;
			} else {
				Serial.println("Non Valid Character " + String(int(c)));
			}
			unsigned int bufLen = buffer.length(); 
			if (bufLen == segmentSize || (currentSize + bufLen) == fileSize) {
				handleFileput(buffer);
				currentSize += bufLen;
				currentSegment += bufLen;
				buffer = "";
				if  (currentSize == fileSize) {
					complete();
					return false;
				} 
				Serial.println("SEGMENT OK " + String(currentSegment));
			}
			return true;
	}
	abort("Unknown Status");
	return false;
}	

void Transfer::complete() {
	transfermode = false;
	file.close();
	reset();
}

void Transfer::start() {
	reset();
	Serial.setRxBufferSize(2000);
	transfermode = true;
	lasttransfer = millis();
}

bool Transfer::handleFileput(String  s) {
	while (buffer.length() % 4 > 0) {
		buffer += "a";
	}
	for (int i = 0; i < buffer.length(); i = i + 4) {
		char c1 = b64.indexOf(buffer[ i + 0]) << 2 | (b64.indexOf(buffer[i + 1]) >> 4);
		char c2 = (b64.indexOf(buffer[i + 1]) & 15) << 4 | (b64.indexOf(buffer[i + 2]) >> 2);
		char c3 = (b64.indexOf(buffer[i + 2]) & 3) << 6 | (b64.indexOf(buffer[i + 3]));
		if (buffer[i + 2] == '=') {
			file.print(c1);
//			currentSize++;
		} else if (buffer[i + 3] == '=') {
			file.print(c1);
			file.print(c2);
//			currentSize += 2;
		} else {
			file.print(c1);
			file.print(c2);
			file.print(c3);
//			currentSize += 3;
		}
		// if (currentSize == fileSize) {
		// 	file.close();
		// 	Serial.println("STATUS: FINISH");
		// 	status = idle;
		// 	return false;
		// }
		// if (currentSize > fileSize) {
		// 	file.close();
		// 	Serial.println("STATUS: FAILED (" + String(currentSize) + "/" + String(fileSize) + ")");
		// 	status = idle;
		// 	return false;
		// }
	}
	return true;
}

Transfer::~Transfer() {
	// TODO Auto-generated destructor stub
}

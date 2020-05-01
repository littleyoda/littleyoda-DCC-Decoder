/*
 * TRANSFER.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef TRANSFER_H_
#define TRANSFER_H_

#include <Arduino.h>
#include <FS.h>
#ifdef ESP32
        #include "SPIFFS.h"
#endif

class Controller;

class Transfer {
public:
	Transfer(Controller* c);
	virtual ~Transfer();
	bool key(char c);
	bool handleFileput(char c);
private:
	String buffer;
	Controller* c;
	 String b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	enum TransferStatus { idle, fileput, finish } status;		
	uint16_t fileSize;
	uint16_t currentSize;
	File file;
};

#endif /* TRANSFER_H_ */

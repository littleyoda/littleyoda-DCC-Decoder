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
	bool handleFileput(String s);
	void abort(String s);
	void reset();
	void complete();
	void loop();
	void start();
	bool transfermode = false;
private:
	String buffer;
	Controller* c;
	String b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	enum TransferStatus { idle, fileput, finish } status;		
	uint32_t currentSegment;
	uint32_t fileSize;
	uint16_t b64Size;
	uint32_t currentSize;
	File file;
	unsigned long lasttransfer;
	uint32_t segmentSize = 500;

};

#endif /* TRANSFER_H_ */

/*
 * ActionDFPlayerMP3.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef ACTIONDFPLAYERMP3_H_
#define ACTIONDFPLAYERMP3_H_

#ifdef ESP8266

#include <SoftwareSerial.h>

#include "INotify.h"
#include "ISettings.h"

class ActionDFPlayerMP3: public INotify,ISettings {
public:
	ActionDFPlayerMP3(int addr, int rx, int tx);
	virtual int loop();
	virtual ~ActionDFPlayerMP3();
	virtual String getHTMLCfg(String urlprefix) override;
	virtual String getHTMLController(String urlprefix) override;
	virtual void setSettings(String key, String value) override;
	virtual void DCCSpeed(int id, int speed, int direction, int source) override;
	virtual void DCCFunc(int id, int bit, int newvalue, int source) override;

private:
	virtual void cmdSelectSDCard();
	virtual void cmdStop();
	virtual void cmdPlay(int number);
	virtual void cmdVolume(int volume);
	virtual void sendCommand(uint8_t command, uint16_t dat);
	virtual void checkReceiveBuffer();
	SoftwareSerial* mp3Serial;
	uint8_t sendBuffer[8];
	uint8_t ansbuf[10];
	int bufferPos = 0;
	int locoId;
	unsigned long lastReceived = 0;
	unsigned long initTime = 0;;
};
;

#endif

#endif /* ACTIONDFPLAYERMP3_H_ */

/*
 * ActionDFPlayerMP3.h
 *
 *  Created on: 21.08.2016
 *      Author: sven
 */

#ifndef ACTIONDFPLAYERMP3_H_
#define ACTIONDFPLAYERMP3_H_

#include <SoftwareSerial.h>
#include "ActionBase.h"

class ActionDFPlayerMP3: public ActionBase {
public:
	ActionDFPlayerMP3(int addr, int rx, int tx);
	virtual int loop();
	virtual ~ActionDFPlayerMP3();
	virtual String getHTMLCfg(String urlprefix);
	virtual String getHTMLController(String urlprefix);
	virtual void setSettings(String key, String value);
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	virtual void DCCFunc(int id, int bit, int newvalue, int source);

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

#endif /* ACTIONDFPLAYERMP3_H_ */

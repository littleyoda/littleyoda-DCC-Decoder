/*
 * ActionSUSIGeneration.h
 *
 *  Created on: 01.11.2017
 *      Author: sven
 */

#ifndef ACTIONSUSIGENERATION_H_
#define ACTIONSUSIGENERATION_H_
#ifdef ESP8266


#include "INotify.h"
#include "ILoop.h"

class ActionSUSIGeneration: public INotify, public ILoop {
public:
	ActionSUSIGeneration(int locoaddr);
	virtual ~ActionSUSIGeneration();
	virtual int loop();
	virtual void DCCSpeed(int id, int speed, int direction, int SpeedSteps, int source);
	virtual void DCCFunc(int id, unsigned long int newvalue, int source);

private:
	void send();
	void fillSpiBuffer();
	void invertBuffer();

	uint16_t LOCO_ADR;
	unsigned char SPIBuf[32];
	uint8_t SPIBufUsed = 0;

	unsigned long int  FUNC_STATE = 0;
	boolean DIR_STATE = false;
	uint8_t SPEED_STATE = 0;
};

#endif
#endif /* ACTIONSUSIGENERATION_H_ */

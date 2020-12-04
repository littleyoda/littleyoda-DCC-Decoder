/*
 * INPUTROTORYENCODER.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INPUTROTORYENCODER_H_
#define INPUTROTORYENCODER_H_

#include <Arduino.h>
#include <LinkedList.h>
#include "Connectors.h"

class InputRotoryEncoder : public Connectors, public ILoop {
public:
	InputRotoryEncoder(ISettings* a, LinkedList<int> *list, String name, int stepvalue, int samplerate);
	virtual ~InputRotoryEncoder() override;
	int loop();

private:
	virtual void GPIOChange(int pin, int newValue);
	uint8_t p1def;
	uint8_t p2def;
	int p1;
	int p2;
	int count = 0;
	int stepvalue;
	int samplerate; // Samplerate or 0 for interrupt driven approach
	String settingName;
	int table[4][4]={
						{0,1,-1,0},
						{-1,0,0,1},
						{1,0,0,-1},
						{0,-1,1,0}
					};
};

#endif /* INPUTROTORYENCODER_H_ */

/*
 * INPUTROTORYENCODER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InputRotoryEncoder.h"
#include "GPIO.h"
#include <FunctionalInterrupt.h>

InputRotoryEncoder::InputRotoryEncoder(ISettings* a, LinkedList<int> *list, String name, int sv, int sr) {
	addAction(a);
	stepvalue = sv;
	samplerate = sr;
	settingName = name;
	GPIOobj.pinMode(list->get(0), INPUT_PULLUP, "Enocder");
	GPIOobj.pinMode(list->get(1), INPUT_PULLUP, "Encoder");
	if (samplerate == 0) {
		GPIOobj.enableInterrupt(list->get(0));
		GPIOobj.enableInterrupt(list->get(1));
	}
	p1def = list->get(0);
	p2def = list->get(1);
	p1 = 0;
	p2 = 0;
	setModulName("RotoryEncoder");
	setConfigDescription("Input: " + GPIOobj.gpio2string(p1def) + "/" + GPIOobj.gpio2string(p2def) + " [" + name + "]");
}

InputRotoryEncoder::~InputRotoryEncoder() {
}

void InputRotoryEncoder::GPIOChange(int pin, int newValue) {
	int lastP1 = p1;
	int lastP2 = p2;
	if (pin == p1def) {
		p1 = newValue;
	}
	if (pin == p2def) {
		p2 = newValue;
	}
	count = count + (table[ lastP1 + lastP2 * 2 ][ p1 + p2 * 2 ] * stepvalue);
}

int InputRotoryEncoder::loop() {
	if (samplerate == 0) {
		if (count == 0) {
			return 50;
		}
		int value = count;
		count = 0;
		send(settingName, String(value));
		return 50;
	} else {
		int value = GPIOobj.digitalRead(p1def);
		GPIOChange(p1def, value);

		value = GPIOobj.digitalRead(p2def);
		GPIOChange(p2def, value);
		return samplerate;
	}
}

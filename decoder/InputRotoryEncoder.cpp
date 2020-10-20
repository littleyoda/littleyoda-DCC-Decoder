/*
 * INPUTROTORYENCODER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InputRotoryEncoder.h"
#include "GPIO.h"
#include <FunctionalInterrupt.h>

InputRotoryEncoder::InputRotoryEncoder(ISettings* a, LinkedList<int> *list, String name) {
	addAction(a);
	settingName = name;
	GPIOobj.pinMode(list->get(0), INPUT_PULLUP, "Enocder");
	GPIOobj.enableInterrupt(list->get(0));

	GPIOobj.pinMode(list->get(1), INPUT_PULLUP, "Encoder");
	GPIOobj.enableInterrupt(list->get(1));

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
	count = count + table[ lastP1 + lastP2 * 2 ][ p1 + p2 * 2 ];

	// Ansatz 1
	// if (pin != p1def && pin != p2def) {
	// 	return;
	// }
	// int lastP1 = p1;
	// int lastP2 = p2;
	// if (pin == p1def) {
	// 	p1 = newValue;
	// }
	// if (pin == p2def) {
	// 	p2 = newValue;
	// }
	// if ((lastP1 == LOW) && (p1 == HIGH)) {
    // 	if (p2 == LOW) {
	// 		count = count + 10;
	//     } else {
	// 		count = count - 10;
    // 	}
	// }
}

int InputRotoryEncoder::loop() {
	if (count == 0) {
		return 200;
	}
	int value = count;
	count = 0;
	send(settingName, String(value));
	return 100;
}

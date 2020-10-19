/*
 * INPUTKEYPADSCHIELD.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 * 
 * 
 */

#include "InputAnalog.h"
#include "Arduino.h"
#include "Logger.h"
InputAnalog::InputAnalog(ISettings* a) {
	addAction(a);
	setModulName("InputAnalog");
	setConfigDescription("Input: A0");
}

InputAnalog::~InputAnalog() {
}

void InputAnalog::addArea(int b, int e, String k, String v) {
  InputAnalogData* d = new InputAnalogData();
  d->beginValue = b;
  d->endValue = e;
  d->key = k;
  d->value = v;
  data.add(d);
}

/**
 * 
 */
int InputAnalog::loop() {
    int value = analogRead(A0);
    bool within = ((lastvalue - 8) < value) && (value < (lastvalue + 8));
    if (!within) {
      Logger::log(LogLevel::ERROR, "Analog: " + String(value));
      lastvalue = value;
    	for (int idx = 0; idx < data.size(); idx++) {
        InputAnalogData* d = data.get(idx);
        if ((d->beginValue <= value) && (value <= d->endValue)) {
            send(d->key, d->value);
        }
      }
    }
    return 200;
}

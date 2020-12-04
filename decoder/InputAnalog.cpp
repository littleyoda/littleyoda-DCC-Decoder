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
#include "GPIO.h"
InputAnalog::InputAnalog(ISettings* a, int pin) {
	addAction(a);
	setModulName("InputAnalog");
	setConfigDescription("Input: " + GPIOobj.gpio2string(pin));
  gpio = pin;
  #ifdef ESP8266
  epsilon = 8;
  #endif
  #ifdef ESP32
  epsilon = 20;
  #endif
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
    int value = analogRead(gpio);
    bool within = ((lastvalue - epsilon) < value) && (value < (lastvalue + epsilon));
    if (!within) {
      Logger::log(LogLevel::TRACE, "INP", getName() + " Analog-Value: " + String(value));
      lastvalue = value;
      if (data.size() == 0) {
        send("analog", String(value));
      } else {
          for (int idx = 0; idx < data.size(); idx++) {
            InputAnalogData* d = data.get(idx);
            if ((d->beginValue <= value) && (value <= d->endValue)) {
                send(d->key, d->value);
            }
          }
      }
    }
    return 50;
}

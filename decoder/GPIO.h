/*
 * GPIO.h
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <Arduino.h>
#include <Wire.h>
#include <map>
#include "Consts.h"
#include "Logger.h"
#include "Pin.h"
#include "ILoop.h"
#include "ISettings.h"
#include "Controller.h"
#include "DataContainer.h"
#include "DataContainerSimpleList.h"
#include "Ports.h"



class GPIOClass : public ILoop {
public:
	DataContainer<int16_t, String>* pinusage;
	// All GPIO in Modus INPUT or INPUT_PULLUP (will update every 30 ms)
	DataContainerSimpleList<int16_t, int16_t>* valueinputpins;



	GPIOClass();
	~GPIOClass();

	String gpio2string(uint16_t gpio);
	int string2gpio(const char* pin);
	int string2gpio(String pin);
	void pinMode(uint16_t pin, uint8_t mode, String usage);
	void pinMode(Pin* pin, uint8_t mode, String usage);

	void digitalWrite(uint16_t pin, uint8_t val);
	void digitalWrite(Pin* pin, uint8_t val);

	int digitalRead(uint16_t pin);
	int digitalRead(Pin* pin);

	void setController(Controller* c);
	void analogWrite(uint16_t pin, int val);
	void analogWriteFreq(uint32_t freq);
	void addMCP23017(uint8_t addr);
	void addPCA9685(uint8_t addr);
  void addArduinoExtender(uint8_t addr, String variant);
	// void add(String s, int pinNumber);
	// void add(String s, int pinNumber, unsigned long supportedFunctions);
	void cache(bool b);
	String getUsage(String sep);
	virtual int loop();
	void debug();

	enum F {
		DIGITAL_INPUT = 1,
		DIGITAL_OUTPUT = 2,
		SUPPORTS_PULLUP = 4,
		SUPPORTS_PULLDOWN = 8,
		UNSTABLE_AT_STARTUP = 16,
		SUPPORTS_PWM = 32,
		SUPPORTS_ADC = 64,
		SUPPORTS_DAC = 128,
		PIN_STRAPPING_AT_STARTUP = 256,
	};

private:
	LinkedList<Ports*>* ports;
	LinkedList<pinInfo*>* pinInfos;
	pinInfo* getPinInfoByGPin(uint16_t gpin);
	Controller* controller;
};

extern GPIOClass GPIOobj;


#endif /* GPIO_H_ */

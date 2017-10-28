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
#include "Adafruit_MCP23017.h"


class GPIOClass {
public:
	typedef std::map<int, String> PinToString;
	PinToString pintostring;

	typedef std::map<String, int> StringToPin;
	StringToPin stringtopin;

	typedef std::map<int, String> PinUsage;
	PinUsage pinusage;


	GPIOClass();
	~GPIOClass();

	String gpio2string(int gpio);
	int string2gpio(const char* pin);
	void pinMode(uint8_t pin, uint8_t mode, String usage);
	void digitalWrite(uint8_t pin, uint8_t val);
	void analogWrite(uint8_t pin, int val);
	void analogWriteFreq(uint32_t freq);
	void enableMCP23017(uint8_t addr);
	void add(String s, int pinNumber);
	String getUsage(String sep);
private:
	Adafruit_MCP23017 *mcp;
	void addUsage(uint8_t pin, String usage);
};

extern GPIOClass GPIO;


#endif /* GPIO_H_ */

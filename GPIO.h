/*
 * GPIO.h
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#ifndef GPIO_H_
#define GPIO_H_


#include <Wire.h>
#include "Consts.h"
#include "Logger.h"
#include "Adafruit_MCP23017.h"


class GPIOClass {
public:
	uint8_t portArray[28] = {D0,   D1,   D2,   D3,   D4,   D5,   D6,   D7,   D8,   D9,   D10,
			100,  101,   102,    103, 104,     105, 106,    107,
			108,  109,   110,    111, 112,     113, 114,    115,
			255
	};
	String portMap[28]   = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10",
			"DA0", "DA1", "DA2", "DA3", "DA4", "DA5", "DA6", "DA7",
			"DB0", "DB1", "DB2", "DB3", "DB4", "DB5", "DB6", "DB7",
			"DISABLE"
	};

	GPIOClass();
	~GPIOClass();
	void init();

	String gpio2string(int gpio);
	int string2gpio(const char* pin);
	void pinMode(uint8_t pin, uint8_t mode);
	void digitalWrite(uint8_t pin, uint8_t val);
	void analogWrite(uint8_t pin, int val);
	void analogWriteFreq(uint32_t freq);

private:
	Adafruit_MCP23017 *mcp;
	int len = 0;

};

extern GPIOClass GPIO;


#endif /* GPIO_H_ */

/*
 * PORTSPCA9685.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */


#include "PortsPCA9685.h"
#include "Adafruit_PWMServoDriver.h"
#include "Logger.h"

PortsPCA9685::PortsPCA9685(uint8_t addr, LinkedList<pinInfo*>* pi, int idx) : Ports(pi, idx) {
	driver = new Adafruit_PWMServoDriver();
	for (int i = 0; i < 16; i++) {
	 	add("E" + String(idx) + "A" + String(i), (idx * 100) + i, i, F::DIGITAL_OUTPUT);
	}
}


PortsPCA9685::~PortsPCA9685() {
	// TODO Auto-generated destructor stub
}


void PortsPCA9685::pinMode(uint16_t pin, uint8_t mode) {
		if (mode != OUTPUT) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Unsupported PinMode: " + String(mode) + " for pin " + String(pin));
			return;
		}
}

int PortsPCA9685::digitalRead(uint16_t pin) {
	return -1;
}


void PortsPCA9685::digitalWrite(uint16_t pin, uint8_t val) {
	if (val == 1) {
		driver->setPWM(pin, 4096, 0);
	} else {
		driver->setPWM(pin, 0, 4096);
	}
}


void PortsPCA9685::analogWrite(uint16_t pin, int val) {

}


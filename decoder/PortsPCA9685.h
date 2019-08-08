/*
 * PORTSPCA9685.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTSPCA9685_H_
#define PORTSPCA9685_H_

#include "Ports.h"
#include "Adafruit_PWMServoDriver.h"


class PortsPCA9685 : public Ports {
public:
	PortsPCA9685(uint8_t addr, LinkedList<pinInfo*>* pi, int idx);
	virtual ~PortsPCA9685();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);

private:
	Adafruit_PWMServoDriver* driver;
//	uint16_t cachedValue;


};

#endif /* PORTSPCA9685_H_ */

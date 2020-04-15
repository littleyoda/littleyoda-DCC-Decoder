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

#define MIN_PULSE_WIDTH 650
#define MAX_PULSE_WIDTH 2350
#define DEFAULT_PULSE_WIDTH 1500
#define FREQUENCY 50

class PortsPCA9685 : public Ports {
public:
	PortsPCA9685(uint8_t addr, LinkedList<pinInfo*>* pi, int idx);
	virtual ~PortsPCA9685();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
  virtual void servoWrite(uint16_t pin, uint8_t val);
private:
	Adafruit_PWMServoDriver* driver;
  int pulseWidth(int angle);
//	uint16_t cachedValue;


};

#endif /* PORTSPCA9685_H_ */

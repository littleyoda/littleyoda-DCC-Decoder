/*
 * PORTSESP8266.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef PORTSESP8266_H_
#define PORTSESP8266_H_

#ifdef ESP8266

#include <Arduino.h>
#include "Ports.h"
#include "Servo.h"
#include "LinkedList.h"

class PortsESP8266 : public Ports {
public:
	PortsESP8266(LinkedList<pinInfo*>* pi, int pinOffset);
	virtual ~PortsESP8266();
	virtual void pinMode(uint16_t pin, uint8_t mode);
	virtual int digitalRead(uint16_t pin);
	virtual void digitalWrite(uint16_t pin, uint8_t val);
	virtual void analogWrite(uint16_t pin, int val);
  virtual void servoWrite(uint16_t pin, uint8_t val);
private:
  LinkedList<Servo*> servoList;
  bool initServo(uint8_t pin);
  
};

#endif  /* ESP8266 */
#endif /* PORTSESP8266_H_ */

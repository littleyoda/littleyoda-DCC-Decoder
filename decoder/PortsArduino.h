
/*
 * PORTSArduino.h
 *
 *  Created on: 14.04.2020
 *      Author: Petr Osipov
 */

#ifndef PORTSARDUINO_H_
#define PORTSARDUINO_H_

#include "Ports.h"
#include "arduinoPortExtender.h"


class PortsArduino : public Ports {
public:
  PortsArduino(uint8_t addr, LinkedList<pinInfo*>* pi, int pinOffset, String variant);
  virtual ~PortsArduino();
  virtual void pinMode(uint16_t pin, uint8_t mode);
  virtual int digitalRead(uint16_t pin);
  virtual void digitalWrite(uint16_t pin, uint8_t val);
  virtual void analogWrite(uint16_t pin, int val);
  virtual void cache(bool c);

private:
  ArduinoPortExtender* extender;
  uint16_t cachedValue;
  uint16_t offset;
};

#endif /* PORTSARDUINO_H_ */

/*
 * PORTSArduino.cpp
 *
 *  Created on: 14.04.2020
 *      Author: Petr Osipov
 */

#include "PortsArduino.h"
#include "Logger.h"

PortsArduino::PortsArduino(uint8_t addr, LinkedList<pinInfo*>* pi, int idx, String variant) : Ports(pi, idx) {
  extender = new ArduinoPortExtender(0x22,0xc0);
  

  offset = idx;
  //TODO: Add MEGA, NodeMCU, Bluepill...
  if (variant == "Uno"||variant == "Nano"){
    add("E" + String(idx) + "A" + String(0), (idx * 100) + 0, 0, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(1), (idx * 100) + 1, 1, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(2), (idx * 100) + 2, 2, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(3), (idx * 100) + 3, 3, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(4), (idx * 100) + 4, 4, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(5), (idx * 100) + 5, 5, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(6), (idx * 100) + 6, 6, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(7), (idx * 100) + 7, 7, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(8), (idx * 100) + 8, 8, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(9), (idx * 100) + 9, 9, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(10), (idx * 100) + 10, 10, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(11), (idx * 100) + 11, 11, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | F::SUPPORTS_PWM);
    add("E" + String(idx) + "A" + String(12), (idx * 100) + 12, 12, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(13), (idx * 100) + 13, 13, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP);
    add("E" + String(idx) + "A" + String(14), (idx * 100) + 14, 14, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | SUPPORTS_ADC);
    add("E" + String(idx) + "A" + String(15), (idx * 100) + 15, 15, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | SUPPORTS_ADC);
    add("E" + String(idx) + "A" + String(16), (idx * 100) + 16, 16, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | SUPPORTS_ADC);
    add("E" + String(idx) + "A" + String(17), (idx * 100) + 17, 17, F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PULLUP | SUPPORTS_ADC);       
  }
  

  

}

PortsArduino::~PortsArduino() {

}

void PortsArduino::pinMode(uint16_t pin, uint8_t mode) {
    pin = pin-(offset * 100);
    if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
      Logger::getInstance()->addToLog(LogLevel::ERROR, "Unsupported PinMode: " + String(mode) + " for pin " + String(pin));
      return;
    }
    if (mode == INPUT_PULLUP) {
      extender->pinMode(pin, PIN_INPUT_PULLUP); 
    }
    else
    if (mode == INPUT){
      extender->pinMode(pin, PIN_INPUT); 
    }
    else
    if (mode == OUTPUT){
      extender->pinMode(pin, PIN_OUTPUT); 
    }
    
    
}

int PortsArduino::digitalRead(uint16_t pin) {
  pin = pin-(offset * 100);
  return extender->digitalRead(pin);
}


void PortsArduino::digitalWrite(uint16_t pin, uint8_t val) {
  pin = pin-(offset * 100);
  uint8_t result = extender->digitalWrite(pin, val);

}


void PortsArduino::analogWrite(uint16_t pin, int val) {
  pin = pin-(offset * 100);
  extender->analogWrite(pin, val);
}

void PortsArduino::cache(bool c) {

}
  

/*
 * PORTSESP8266.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "PortsESP8266.h"
#include "Consts.h"

#ifdef ESP8266


PortsESP8266::PortsESP8266(LinkedList<pinInfo*>* pi, int pinOffset) : Ports(pi, pinOffset) {
	add("DISABLE", Consts::DISABLE, Consts::DISABLE, 0);
	add("disable", Consts::DISABLE, Consts::DISABLE, 0);
	add("disabled", Consts::DISABLE, Consts::DISABLE, 0);

	add("A0", 17,F::SUPPORTS_ADC);

	unsigned long defaultPin = F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM;

#ifdef ARDUINO_ESP8266_ESP01
	add("D0", 16,0);
	add("D2", 4,0);
	add("D3", 0,0);
	add("D4", 2,0);
	add("D5", 14,0);
	add("D7", 13,0);
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
	add("D0", D0,F::DIGITAL_INPUT | F::DIGITAL_OUTPUT); // NO PWM, NO I2C, No Interrupt 
	add("D1", D1, defaultPin); // DEFAULT I²C => SCL
	add("D2", D2, defaultPin); // DEFAULT I²C => SDA
	add("D3", D3, F::PIN_STRAPPING_AT_STARTUP | F::UNSTABLE_AT_STARTUP | F::PULL_UP | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 0
	add("D4", D4, F::PIN_STRAPPING_AT_STARTUP | F::UNSTABLE_AT_STARTUP | F::PULL_UP | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 2
	add("D5", D5, defaultPin);
	add("D6", D6, defaultPin);
	add("D7", D7, defaultPin);
	add("D8", D8, F::PIN_STRAPPING_AT_STARTUP | F::PULL_DOWN | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 15
#endif
#ifdef ARDUINO_ESP8266_NODEMCU
	add("D0", D0,F::DIGITAL_INPUT | F::DIGITAL_OUTPUT); // NO PWM, NO I2C, No Interrupt 
	add("D1", D1, defaultPin); // DEFAULT I²C => SCL
	add("D2", D2, defaultPin); // DEFAULT I²C => SDA
	add("D3", D3, F::PIN_STRAPPING_AT_STARTUP | F::UNSTABLE_AT_STARTUP | F::PULL_UP | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 0
	add("D4", D4, F::PIN_STRAPPING_AT_STARTUP | F::UNSTABLE_AT_STARTUP | F::PULL_UP | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 2
	add("D5", D5, defaultPin);
	add("D6", D6, defaultPin);
	add("D7", D7, defaultPin);
	add("D8", D8, F::PIN_STRAPPING_AT_STARTUP | F::PULL_DOWN  | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM); // GPIO 15
	add("D9", D9,0);
	add("D10", D10,0);
#endif
}

PortsESP8266::~PortsESP8266() {
	// TODO Auto-generated destructor stub
}

void PortsESP8266::pinMode(uint16_t pin, uint8_t mode) {
	::pinMode(pin, mode);
}

int PortsESP8266::digitalRead(uint16_t pin) {
	return ::digitalRead(pin);
}
void PortsESP8266::digitalWrite(uint16_t pin, uint8_t val) {
	::digitalWrite(pin, val);
}
void PortsESP8266::analogWrite(uint16_t pin, int val) {
	::analogWrite(pin, val);
}

bool PortsESP8266::initServo(uint8_t pin){
  if (!servoList.get(pin)){
    Servo* srv = new Servo();
    servoList.add(pin, srv);
  }
  return true;
}

void PortsESP8266::servoWrite(uint16_t pin, uint8_t val) {
  
  Servo* s = servoList.get(pin);
  if (!s){
    bool worked = initServo(pin);
    if (!worked){
      return;
    }
    else
    {
      s = servoList.get(pin);
      if (!s){
        return;
      }
    }
  }
  s->attach(pin);
  delay(15);
  s->write(val);
  delay(15);
}
#endif

/*
 * PORTSESP32.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "PortsESP32.h"
#include "Consts.h"
#include "Logger.h"

#ifdef ESP32
#include "ESP32_Servo.h"

PortsESP32::PortsESP32(LinkedList<pinInfo*>* pi, int pinOffset) : Ports(pi, pinOffset) {
	add("DISABLE", Consts::DISABLE, Consts::DISABLE, 0);
	add("disable", Consts::DISABLE, Consts::DISABLE, 0);
	add("disabled", Consts::DISABLE, Consts::DISABLE, 0);
	#ifdef ARDUINO_MH_ET_LIVE_ESP32MINIKIT
		int pins[31] = {  /* Row 1 */ 11,  9, 14, 34, 33, 35, 39, 
						  /* Row 2 */10, 13,  5, 23, 19, 18, 26, 	
						  /* Row 3 */ 6,  8,  2,  0,  4, 12, 32, 25, 27,
						  /* Row 4*/ 7, 15, 16, 17, 21, 22, 3, 1
		}; 
		int len = 31;
	#else
		int pins[34] = {  
							0,  1,  2,  3,  4, 		5, 12, 13, 14, 15, 
							16, 17, 18, 19, 20,		21, 22, 23, 24, 25, 
							26, 27, 28, 29, 30,		31, 32, 33, 34, 35, 
							36, 37, 38, 39
		};
		int len = 34;

	#endif
	for (int i = 0; i < len; i++) {
			addESP32Pin(i);
	}
}


PortsESP32::~PortsESP32() {
	// TODO Auto-generated destructor stub
}

void PortsESP32::pinMode(uint16_t pin, uint8_t mode) {
	::pinMode(pin, mode);
}

int PortsESP32::digitalRead(uint16_t pin) {
		return ::digitalRead(pin);
}

void PortsESP32::digitalWrite(uint16_t pin, uint8_t val) {
	::digitalWrite(pin, val);
}
void PortsESP32::analogWrite(uint16_t pin, int val) {
	Serial.println("Analogwrite not supported");
}
bool PortsESP32::initServo(uint8_t pin){
  if (!servoList.get(pin)){
    Servo* srv = new Servo();
    servoList.add(pin, srv);
  }
  return true;
}

void PortsESP32::servoWrite(uint16_t pin, uint8_t val) {
  
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

void PortsESP32::addESP32Pin(int x)
{
	// GPIO 0,2,4, 12,13,14,15,  25,26,27 Pin are only usable for ADC if Wifi is disabled
	switch (x)
	{
	case 0:
		add("IO00", 0, F::PIN_STRAPPING_AT_STARTUP /* F::SUPPORTS_ADC*/);
		break;
	case 1:
		add("IO01", 1, F::UNSTABLE_AT_STARTUP);
		break;
	case 2:
		add("IO02", 2, F::PIN_STRAPPING_AT_STARTUP /*F::SUPPORTS_ADC*/);
		break;
	case 3:
		add("IO03", 3, F::UNSTABLE_AT_STARTUP);
		break;
	case 4:
		add("IO04", 4, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM /*| F::SUPPORTS_ADC*/);
		break;
	case 5:
		add("IO05", 5, F::UNSTABLE_AT_STARTUP | F::PIN_STRAPPING_AT_STARTUP);
		break;
	// add("IO06", 6);
	// add("IO07", 7);
	// add("IO08", 8);
	// add("IO09", 9);
	// add("IO10", 10);
	// add("IO11", 11);
	case 12:
		add("IO12", 12, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM | F::PIN_STRAPPING_AT_STARTUP /*| F::SUPPORTS_ADC*/);
		break;
	case 13:
		add("IO13", 13, 0 /* F::SUPPORTS_ADC */);
		break;
	case 14:
		add("IO14", 14, F::UNSTABLE_AT_STARTUP /*| F::SUPPORTS_ADC*/);
		break;
	case 15:
		add("IO15", 15, F::UNSTABLE_AT_STARTUP /*| F::SUPPORTS_ADC */ | F::PIN_STRAPPING_AT_STARTUP);
		break;
	case 16:
		add("IO16", 16, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 17:
		add("IO17", 17, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 18:
		add("IO18", 18, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 19:
		add("IO19", 19, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 20:
		add("IO20", 20, 0);
		break;
	case 21:
		add("IO21", 21, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 22:
		add("IO22", 22, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 23:
		add("IO23", 23, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM);
		break;
	case 24:
		add("IO24", 24, 0);
		break;
	case 25:
		add("IO25", 25, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM /*| F::SUPPORTS_ADC*/ | F::SUPPORTS_DAC);
		break;
	case 26:
		add("IO26", 26, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM /*| F::SUPPORTS_ADC*/ | F::SUPPORTS_DAC);
		break;
	case 27:
		add("IO27", 27, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM /*| F::SUPPORTS_ADC*/);
		break;
	case 28:
		add("IO28", 28, 0);
		break;
	case 29:
		add("IO29", 29, 0);
		break;
	case 30:
		add("IO30", 30, 0);
		break;
	case 31:
		add("IO31", 31, 0);
		break;
	case 32:
		add("IO32", 32, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM | F::SUPPORTS_ADC);
		break;
	case 33:
		add("IO33", 33, F::SUPPORTS_PULLDOWN | F::SUPPORTS_PULLUP | F::DIGITAL_INPUT | F::DIGITAL_OUTPUT | F::SUPPORTS_PWM | F::SUPPORTS_ADC);
		break;
	case 34:
		add("IO34", 34, F::DIGITAL_INPUT | F::SUPPORTS_ADC); // No PULLDUP/DOWNS
		break;
	case 35:
		add("IO35", 35, F::DIGITAL_INPUT | F::SUPPORTS_ADC); // No PULLDUP/DOWNS
		break;
	case 36:
		add("IO36", 36, F::DIGITAL_INPUT | F::SUPPORTS_ADC); // No PULLDUP/DOWNS
		break;
	case 37:
		add("IO37", 37, F::DIGITAL_INPUT | F::SUPPORTS_ADC); // ?
		break;
	case 38:
		add("IO38", 38, F::DIGITAL_INPUT | F::SUPPORTS_ADC); // ?
		break;
	case 39:
		add("IO39", 39, F::DIGITAL_INPUT | F::SUPPORTS_ADC);
		break;
	default:
		Logger::log(LogLevel::ERROR, "Unbekannter ESP32-PIN: " + String(x));
	}
}

#endif

/*
 * GPIO.cppgpg
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#include <FunctionalInterrupt.h>
#include "GPIO.h"
#include "PortsESP8266.h"
#include "PortsESP32.h"
#include "PortsMCP23017.h"
#include "PortsPCA9685.h"
#include "PortsArduino.h"

GPIOClass::GPIOClass() {
	ports = new LinkedList<Ports*>();
	pinInfos = new LinkedList<pinInfo*>();
	valueinputpins = new DataContainerSimpleList<int16_t, int16_t>(16 * 6 + 12, -1, -1);
	#ifdef  ESP8266
		Ports* p = new PortsESP8266(pinInfos, 0);
		ports->add(p);
	#endif
	#ifdef ESP32
		Serial.println("ESP32");
		Ports* p = new PortsESP32(pinInfos, 0);
		ports->add(p);
	#endif
}


GPIOClass::~GPIOClass() {
}

/**
 * 14 => "D4"
 */
String GPIOClass::gpio2string(uint16_t gpio) {
	for (int i = 0; i < pinInfos->size(); i++) {
		if (pinInfos->get(i)->gPinNummer == gpio) {
			return pinInfos->get(i)->name;
		}
	}
 	Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekannter GPIO: " + String(gpio));
	return "Pin " + String(gpio);
}

void GPIOClass::debug() {
	for (int i = 0; i < pinInfos->size(); i++) {
		pinInfo* p = pinInfos->get(i);
		Serial.println(String(p->name) + " " + String(p->pinNumber) + " " + String(p->gPinNummer) + ": " + p->usage);
	}
}

/**
 * "D4" => 14
 */
int GPIOClass::string2gpio(const char *pin) {
	if (pin == NULL) {
		Logger::log(LogLevel::ERROR, "PIN fehlt (null in string2gpio)");
		return Consts::DISABLE;
	}
	for (int i = 0; i < pinInfos->size(); i++) {
		if (pinInfos->get(i)->name.equals(pin)) {
			return pinInfos->get(i)->gPinNummer;
		}
	}
	Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekannter Pin in Config: " + String(pin));
	return Consts::DISABLE;
}


pinInfo* GPIOClass::getPinInfoByGPin(uint16_t gpin) {
	for (int i = 0; i < pinInfos->size(); i++) {
		if (pinInfos->get(i)->gPinNummer == gpin) {
			return pinInfos->get(i);
		}
	}
	return nullptr;
}
int GPIOClass::string2gpio(String pin) {
	return string2gpio(pin.c_str());
}


void GPIOClass::enableInterrupt(uint16_t pin) {
	attachInterrupt(pin, std::bind(&GPIOClass::intCallbackValue,this, pin, 1), RISING );
	attachInterrupt(pin, std::bind(&GPIOClass::intCallbackValue,this, pin, 0), FALLING );
}

void GPIOClass::enableInterrupt(Pin* pin) {
	enableInterrupt(pin->getPin());
}

void GPIOClass::intCallbackValue(uint16_t pin, int value) {
	int oldval = valueinputpins->getValue(pin);
	if (value != oldval) {
		controller->notifyGPIOChange(pin, value);
		valueinputpins->put(pin, value);
	}
}

void GPIOClass::intCallback(uint16_t pin) {
	int oldval = valueinputpins->getValue(pin);
	int val = digitalRead(pin);
	if (val != oldval) {
		controller->notifyGPIOChange(pin, val);
		valueinputpins->put(pin, val);
	}
}

void GPIOClass::pinMode(Pin *pin, uint8_t mode, String usage) {
	pinMode(pin->getPin(), mode, usage);
}

void GPIOClass::pinMode(uint16_t pin, uint8_t mode, String usage) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	pinInfo* pi = getPinInfoByGPin(pin);
	if (pi == nullptr) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Unbekannter Pin (pinMode): " + String(pin));
		return;
	}
	pi->pinController->pinMode(pi->pinNumber, mode);
	if (mode == INPUT || mode == INPUT_PULLUP) {
		int v = digitalRead(pin);
		valueinputpins->put(pin, v);
	}
	if (pi->usage.length() > 0) {
		pi->usage = pi->usage + ";" + usage;
	} else {
		pi->usage = usage;

	}
}

void GPIOClass::digitalWrite(Pin *pin, uint8_t val) {
	if (pin->isInvert()) {
		if (val == 0) {
			val = 1;
		}
		else if (val == 1) {
			val = 0;
		}
	}
	digitalWrite(pin->getPin(), val);
}

void GPIOClass::digitalWrite(uint16_t pin, uint8_t val) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	pinInfo* pi = getPinInfoByGPin(pin);
	if (pi == nullptr) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Unbekannter Pin (pinMode): " + String(pin));
		return;
	}
	pi->pinController->digitalWrite(pin, val);
}

void GPIOClass::servoWrite(Pin *pin, uint8_t val) {
  
  digitalWrite(pin->getPin(), val);
}

void GPIOClass::servoWrite(uint16_t pin, uint8_t val) {
  if (pin == Consts::DISABLE) {
    Logger::getInstance()->addToLog(LogLevel::ERROR,
                    "Accessing Disabled Pin (pinMode): " + String(pin));
    return;
  }
  pinInfo* pi = getPinInfoByGPin(pin);
  if (pi == nullptr) {
    Logger::getInstance()->addToLog(LogLevel::ERROR,
                    "Unbekannter Pin (pinMode): " + String(pin));
    return;
  }
  pi->pinController->servoWrite(pin, val);
}

void GPIOClass::analogWrite(uint16_t pin, int val) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Accessing Disabled Pin (analogwrite): " + String(pin));
		return;
	}
	pinInfo* pi = getPinInfoByGPin(pin);
	if (pi == nullptr) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Unbekannter Pin (analogwrite): " + String(pin));
		return;
	}
	pi->pinController->analogWrite(pi->pinNumber, val);
}

void GPIOClass::analogWriteFreq(uint32_t freq) {
#ifdef ESP8266
	::analogWriteFreq(freq);
#elif ESP32
	// Hack
#endif
}


void GPIOClass::addMCP23017(uint8_t addr) {
	Ports* p = new PortsMCP23017(addr, pinInfos, ports->size());
	ports->add(p);
}

void GPIOClass::addPCA9685(uint8_t addr) {
	Ports* p = new PortsPCA9685(addr, pinInfos, ports->size());
	ports->add(p);
}
void GPIOClass::addArduinoExtender(uint8_t addr, String variant){
  Ports* p = new PortsArduino(addr, pinInfos, ports->size(), variant);
  ports->add(p);
}

GPIOClass GPIOobj;

int GPIOClass::digitalRead(uint16_t pin) {
	pinInfo* pi = getPinInfoByGPin(pin);
	if (pi == nullptr) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Unbekannter Pin (analogwrite): " + String(pin));
		return 0;
	}
	return pi->pinController->digitalRead(pi->pinNumber);
}

int GPIOClass::digitalRead(Pin *pin) {
	int v = digitalRead(pin->getPin());
	if (pin->isInvert()) {
		if (v == 0) {
			v = 1;
		}
		else if (v == 1) {
			v = 0;
		}
	}
	return v;
}

void GPIOClass::setController(Controller *c) {
	controller = c;
}

void GPIOClass::cache(bool b) {
	// TODO Cache for all
}


String GPIOClass::getUsage(String sep) {
	//TODO	String out = data->toString(sep) + sep + pinusage->toString(sep) + sep;
	String out = "";

	//	TODO for (std::map<int, int>::iterator i = valueinputpins.begin (); i != valueinputpins.end (); i++) {
	//		int pin = (*i).first;
	//		int oldval = (*i).second;
	//		out += "Pin: " + String(pin) + " Status: " + String(oldval) + sep;
	//	}
	return out;
}

int GPIOClass::loop() {
	cache(true);
	for (int i = 0; i < valueinputpins->used(); i++) {
		int pin = valueinputpins->getKey(i);
		int oldval = valueinputpins->getValue(i);
		int val = digitalRead(pin);
		if (val != oldval) {
			controller->notifyGPIOChange(pin, val);
			valueinputpins->put(pin, val);
		}
	}
	cache(false);
	return 30;
}

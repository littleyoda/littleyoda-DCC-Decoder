/*
 * GPIO.cppgpg
 *
 *  Created on: 15.07.2017
 *      Author: sven
 */

#include "GPIO.h"

GPIOClass::GPIOClass()
{
	mcps = new LinkedList<Adafruit_MCP23017 *>();
	data = new DataContainerSimpleList<String, int16_t>(16 * 6 + 12, "", -1);
	valueinputpins = new DataContainerSimpleList<int16_t, int16_t>(16 * 6 + 12, -1, -1);
	pinusage = new DataContainer<int16_t, String>(-1, "");
	cacheEnabled = false;
	cachedValue = NULL;
#ifdef ARDUINO_ESP8266_ESP01
	add("D0", 16);
	add("D2", 4);
	add("D3", 0);
	add("D4", 2);
	add("D5", 14);
	add("D7", 13);
#endif
#ifdef ARDUINO_ESP8266_WEMOS_D1MINI
	add("D0", D0);
	add("D1", D1);
	add("D2", D2);
	add("D3", D3);
	add("D4", D4);
	add("D5", D5);
	add("D6", D6);
	add("D7", D7);
	add("D8", D8);
#endif
#ifdef ARDUINO_ESP8266_NODEMCU
	add("D0", D0);
	add("D1", D1);
	add("D2", D2);
	add("D3", D3);
	add("D4", D4);
	add("D5", D5);
	add("D6", D6);
	add("D7", D7);
	add("D8", D8);
	add("D9", D9);
	add("D10", D10);
#endif
#ifdef ESP32
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
#endif
	add("DISABLE", Consts::DISABLE);
	add("disable", Consts::DISABLE);
	add("disabled", Consts::DISABLE);
}

#ifdef ESP32
void GPIOClass::addESP32Pin(int x)
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
		add("IO20", 20);
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
		add("IO24", 24);
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
		add("IO28", 28);
		break;
	case 29:
		add("IO29", 29);
		break;
	case 30:
		add("IO30", 30);
		break;
	case 31:
		add("IO31", 31);
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
/**
 * "D4", 14
 */
void GPIOClass::add(String s, int pinNumber)
{
	data->put(s, pinNumber);
}

/**
 * "D4", 14
 */
void GPIOClass::add(String s, int pinNumber, unsigned long supportedFunctions) {
	data->put(s, pinNumber);
	// TODO
}

GPIOClass::~GPIOClass() {
}

/**
 * 14 => "D4"
 */
String GPIOClass::gpio2string(int gpio) {
	if (!data->containsValue(gpio))	{
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekannter GPIO: " + String(gpio));
		return "Pin " + String(gpio);
	}
	return data->getKeyByValue(gpio);
}

/**
 * "D4" => 14
 */
int GPIOClass::string2gpio(const char *pin) {
	if (pin == NULL) {
		Logger::log(LogLevel::ERROR, "PIN fehlt (null in string2gpio)");
		return Consts::DISABLE;
	}
	String s = String(pin);
	if (!data->containsKey(s)) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekannter Pin in Config: " + s);
		return Consts::DISABLE;
	}
	return data->getValueByKey(s);
}

int GPIOClass::string2gpio(String pin) {
	if (pin == NULL) {
		Logger::log(LogLevel::ERROR, "PIN fehlt (null in string2gpio)");
		return Consts::DISABLE;
	}
	if (!data->containsKey(pin)) {
		Logger::getInstance()->addToLog(LogLevel::ERROR, "Unbekannter Pin in Config: " + pin);
		return Consts::DISABLE;
	}
	return data->getValueByKey(pin);
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
	if (pin >= 100) {
		if (mode != INPUT && mode != OUTPUT && mode != INPUT_PULLUP) {
			Logger::getInstance()->addToLog(LogLevel::ERROR, "Unsupported PinMode: " + String(mode) + " for pin " + String(pin));
			return;
		}
		int mcpIdx = (pin / 100) - 1;
		int realPin = pin % 100;
		Adafruit_MCP23017 *mcp = mcps->get(mcpIdx);
		if (mode == INPUT_PULLUP) {
			mcp->pinMode(realPin, INPUT);
			mcp->pullUp(realPin, HIGH);
		} else {
			mcp->pinMode(realPin, mode);
		}
	} else {
		::pinMode(pin, mode);
	}
	if (mode == INPUT || mode == INPUT_PULLUP) {
		int v = digitalRead(pin);
		valueinputpins->put(pin, v);
	}
	addUsage(pin, usage);
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
	if (pin >= 100) {
		int mcpIdx = (pin / 100) - 1;
		int realPin = pin % 100;
		return mcps->get(mcpIdx)->digitalWrite(realPin, val);
	} else {
		::digitalWrite(pin, val);
	}
}

void GPIOClass::analogWrite(uint16_t pin, int val) {
	if (pin == Consts::DISABLE) {
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Accessing Disabled Pin (pinMode): " + String(pin));
		return;
	}
	if (pin >= 100) 
		Logger::getInstance()->addToLog(LogLevel::ERROR,
										"Analog Write not possible for Pin: " + String(pin));
		return;
	}
#ifdef ESP8266
	::analogWrite(pin, val);
#elif ESP32
	// Hack
#endif
}

void GPIOClass::analogWriteFreq(uint32_t freq) {
#ifdef ESP8266
	::analogWriteFreq(freq);
#elif ESP32
	// Hack
#endif
}

/**
 * Fügt die sprechenden PIN Bezeichner (ExA0 bis ExB7) für die MCP23017 hinzu
 */

void GPIOClass::addMCP23017(uint8_t addr) {
	Adafruit_MCP23017 *m = new Adafruit_MCP23017();
	int idx = mcps->size();
	int offset = (idx + 1) * 100;
	m->begin(addr);
	for (int i = 0; i < 8; i++) {
		add("E" + String(idx) + "A" + String(i), offset + i);
		add("E" + String(idx) + "B" + String(i), offset + 8 + i);
	}
	mcps->add(m);
}

GPIOClass GPIOobj;

int GPIOClass::digitalRead(uint16_t pin) {
	if (pin >= 100) {
		int mcpIdx = (pin / 100) - 1;
		int realPin = pin % 100;
		if (cacheEnabled) {
			uint16_t t = cachedValue[mcpIdx];
			t = (t >> realPin) & 1;
			return (int)t;
		} else {
			return mcps->get(mcpIdx)->digitalRead(realPin);
		}
	}
	else {
		return ::digitalRead(pin);
	}
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
	cacheEnabled = b;
	if (b) {
		int size = mcps->size();
		if (cachedValue == NULL) {
			cachedValue = new uint16_t[size];
		}
		for (int i = 0; i < size; i++) {
			cachedValue[i] = mcps->get(i)->readGPIOAB();
		}
	}
}

void GPIOClass::addUsage(uint16_t pin, String usage) {
	return;
	Logger::log(LogLevel::DEBUG, "Adding " + usage + " to " + String(pin));
	String value = usage;
	if (pinusage->containsKey(pin)) {
		String oldvalue = pinusage->getValueByKey(pin);
		value = oldvalue + "; " + value;
		pinusage->removeByKey(pin);
	}
	pinusage->put(pin, value);
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

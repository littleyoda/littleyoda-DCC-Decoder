/*
 * INPUTKEYPADSCHIELD.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef INPUTKEYPADSCHIELD_H_
#define INPUTKEYPADSCHIELD_H_

#include "ILoop.h"
#include "Connectors.h"
#include "ISettings.h"

struct InputAnalogData{
  int beginValue;
  int endValue;
  String key;
  String value;
};

class InputAnalog : public ILoop, public Connectors {
public:
	InputAnalog(ISettings* a, int pin);
	virtual ~InputAnalog();
	int loop();
	void addArea(int b, int e, String key, String value);
private:
	int gpio;
	int lastvalue = 0;
	LinkedList<InputAnalogData*> data;
	int epsilon;

};

#endif /* INPUTKEYPADSCHIELD_H_ */

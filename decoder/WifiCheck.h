/*
 * WIFICHECK.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifndef WIFICHECK_H_
#define WIFICHECK_H_

#include "ILoop.h"
#include "Controller.h"

class WifiCheck: public ILoop {
public:
	WifiCheck(Controller* c);
	virtual ~WifiCheck();
	virtual int loop();
	static int wifiGetOpMode();
	static bool wifiISAPActive();
    static uint8_t getAPChannel();
	static int32_t getWifiChannel();

private:
	int lastWifiStatus = 6;
	Controller* controll;
};

#endif /* WIFICHECK_H_ */

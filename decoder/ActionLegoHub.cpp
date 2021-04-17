/*
 * ACTIONLEGOHUB.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */
#ifdef LY_FEATURE_LEGO
#include "ActionLegoHub.h"
#include "Logger.h"

ActionLegoHub::ActionLegoHub()
{
	legoinoHub.init();
	delay(10);
	// TODO Auto-generated constructor stub
}

ActionLegoHub::~ActionLegoHub()
{
	// TODO Auto-generated destructor stub
}

void ActionLegoHub::setSettings(String key, String value)
{
	Logger::log(LogLevel::TRACE, "SetSettings " + key + "/" + value);
	if (!legoinoHub.isConnected()) {
		Logger::log(LogLevel::INFO, "SetSettings " + key + "/" + value + " but not connected!");
		return;
	} 
	if (key.equals("sd")) {
		int speed = value.toInt();
		if (speed == 0) {
			legoinoHub.stopBasicMotor(port);
		} else {
			if (speed > 100) {
				speed = 100;
			} else if (speed < -100) {
				speed = -100;
			}
			legoinoHub.setBasicMotorSpeed(port, speed);
		}
	}
}

int ActionLegoHub::loop() {

	if (legoinoHub.isConnecting()) {
		Logger::log(LogLevel::INFO, "Trying to connect to the Lego Hub");
		legoinoHub.connectHub();
		if (legoinoHub.isConnected() && !isInitialized) {
			Logger::log(LogLevel::INFO, "Connected to Lego HUB");
			isInitialized = true;
		} else {
			Logger::log(LogLevel::ERROR,"Failed to connect to HUB");
			return 1000;
		}
	} else {
		if (isInitialized) {
			Logger::log(LogLevel::ERROR,"Lego Hub: Connection lost");
			isInitialized = false;
		}
	}
	return 1000;
}
#endif
/*
 * ACTIONLEGOHUB.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifdef LY_FEATURE_LEGO

#ifndef ACTIONLEGOHUB_H_
#define ACTIONLEGOHUB_H_

#include "ISettings.h"
#include "Lpf2Hub.h"
#include "ILoop.h"

class ActionLegoHub : public ISettings, public ILoop {
public:
	ActionLegoHub();
	virtual ~ActionLegoHub();
	virtual void setSettings(String key, String value) override;
	virtual int loop() override;

private:
	 Lpf2Hub legoinoHub;
	 bool isInitialized = false;
	 byte port = (byte)PoweredUpHubPort::A;
	 
};

#endif /* ACTIONLEGOHUB_H_ */
#endif
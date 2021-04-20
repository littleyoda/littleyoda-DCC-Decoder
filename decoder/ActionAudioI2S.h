/*
 * ACTIONAUDIOI2S.h
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#ifdef LY_FEATURE_AUDIO

#ifndef ACTIONAUDIOI2S_H_
#define ACTIONAUDIOI2S_H_

#include <WiFi.h> 
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include <AudioOutputI2S.h>
#include <AudioGenerator.h>
#include <AudioFileSource.h>
#include <HTTPClient.h>

#include "ISettings.h"
#include "ILoop.h"

class ActionAudioI2S : public ISettings, public ILoop {
public:
	ActionAudioI2S(bool useSD);
	virtual ~ActionAudioI2S();
	virtual int loop() override;
	virtual void setSettings(String key, String value) override;
	virtual String getHTMLController(String urlprefix) override;
 	virtual void getHTMLConfig(String urlprefix, Controller* c) override;

private:
	AudioOutputI2S *out;
	AudioGenerator *generator;
	AudioFileSource *file;
	void play(int idx);
	boolean useSD = false;
	FS* source;
};

 


#endif /* ACTIONAUDIOI2S_H_ */

#endif
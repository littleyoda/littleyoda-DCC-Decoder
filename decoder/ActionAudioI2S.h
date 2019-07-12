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
#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <SD.h>
#include "ActionAudioI2S.h"
#include <AudioOutputI2S.h>
#include <AudioFileSourcePROGMEM.h>
#include <AudioGeneratorFLAC.h>
#include <AudioFileSourceSPIFFS.h>
#include "ISettings.h"
#include "ILoop.h"

 
class ActionAudioI2S : public ISettings, public ILoop {
public:
	ActionAudioI2S();
	virtual ~ActionAudioI2S();
	virtual int loop();
	virtual void setSettings(String key, String value);
	virtual String getHTMLCfg(String urlprefix) ;
	virtual String getHTMLController(String urlprefix);
 
private:
	AudioOutputI2S *out;
	AudioGenerator *generator;
  	AudioFileSourceSPIFFS *file;
	void play(int idx);
};

 


#endif /* ACTIONAUDIOI2S_H_ */

#endif
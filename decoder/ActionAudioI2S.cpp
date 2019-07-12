/*
 * ACTIONAUDIOI2S.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */
#ifdef LY_FEATURE_AUDIO

#include "ActionAudioI2S.h"
#include "Logger.h"
#include <AudioGeneratorWAV.h>
#include <AudioGeneratorMP3.h>


ActionAudioI2S::ActionAudioI2S() {
  out = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 10);
  out->SetGain(0.7);
  generator = nullptr;
  file = nullptr;
}

ActionAudioI2S::~ActionAudioI2S() {
}

int ActionAudioI2S::loop() {  
  if (generator != nullptr && generator->isRunning()) {
    if (!generator->loop()) { 
      generator->stop();
    }
    return 1;
  } 
  return 1000;
}


void ActionAudioI2S::setSettings(String key, String value) {
	int status = value.toInt();
  if (status == 0) {
    return;
  }
	Logger::getInstance()->addToLog(LogLevel::TRACE, "Audio Value: " + value);
  play(status);
 }

void ActionAudioI2S::play(int idx) {
  if (generator != nullptr && generator->isRunning()) {   
    generator->stop();
  }
  delete(generator);
  delete(file);
  String filename = "";
  if (SPIFFS.exists("/" + String(idx) + ".mp3")) {
    filename = "/" + String(idx) + ".mp3";
    generator = new AudioGeneratorMP3();
  } else if (SPIFFS.exists("/" + String(idx) + ".wav")) {
    filename = "/" + String(idx) + ".wav";
    generator = new AudioGeneratorWAV();
  } else {
      Serial.println("Audiodatei für " + String(idx) + " nicht gefunden!");
      return;
  }
  file = new AudioFileSourceSPIFFS(filename.c_str());
  generator->begin(file, out);  
}

String  ActionAudioI2S::getHTMLCfg(String urlprefix) {
	return "";
}

String  ActionAudioI2S::getHTMLController(String urlprefix) {
	String message =  "<div class=\"row\"> <div class=\"column column-10\">";
	message += "Sound ";
	message += "</div><div class=\"column column-90\">";


	for (int i = 1; i <= 20; i++) {
		String action = "send('" + urlprefix + "&key=idx&value=" + i + "')";
		message += "<a href=\"#\" onclick=\"";
		message += action;
		message += "\">Song ";
		message += String(i);
		message += "°</a> \r\n";
	}
	message += "</div>";
	message += "</div>";
	return message;
}


#endif

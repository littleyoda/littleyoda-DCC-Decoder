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
#include <AudioFileSourceFS.h>
#include "Controller.h"

void StatusCallback(void *cbData, int code, const char *string)
{
  const char *ptr = reinterpret_cast<const char *>(cbData);
  Serial.printf("STATUS(%s) '%d' = '%s'\r\n", ptr, code, string);
 }


ActionAudioI2S::ActionAudioI2S(bool _useSD) {
  out = new AudioOutputI2S(0, AudioOutputI2S::EXTERNAL_I2S, 10);
  out->SetGain(0.7);
  generator = nullptr;
  file = nullptr;
  if (_useSD) {
    source = &SD;
  } else {
    source = &SPIFFS;
  }
  useSD = _useSD;
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
  return 200;
}



void ActionAudioI2S::setSettings(String key, String value) {
	int status = value.toInt();
	Logger::getInstance()->addToLog(LogLevel::TRACE, "Audio Value: " + value + " " + key);
  if (key == "songid") {
    if (status == 0) {
      return;
    }
    play(status);
 }
 if (key == "volume") {
    out->SetGain(status / 25.0f);
 }
}

void ActionAudioI2S::play(int idx) {
  if (generator != nullptr && generator->isRunning()) {   
    generator->stop();
  }
  delete(generator); generator = nullptr;
  delete(file); file = nullptr;
  String filename = "";
  if (source->exists("/" + String(idx) + ".mp3")) {
    filename = "/" + String(idx) + ".mp3";
    generator = new AudioGeneratorMP3();
  } else if (source->exists("/" + String(idx) + ".wav")) {
    filename = "/" + String(idx) + ".wav";
    generator = new AudioGeneratorWAV();
  } else {
    Logger::log(LogLevel::ERROR, "Audiodatei für " + String(idx) + " nicht gefunden!");
    return;
  }
  file = new AudioFileSourceFS(*source, filename.c_str());
  file->RegisterStatusCB(StatusCallback, (void*)"file");
  generator->RegisterStatusCB(StatusCallback, (void*)"generator");
  generator->begin(file, out);
  generator->loop();
}


String  ActionAudioI2S::getHTMLController(String urlprefix) {
/*       String message =  "<div class=\"row\"> <div class=\"column column-10\">";
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
       return message;*/
       return "";
}

void ActionAudioI2S::getHTMLConfig(String urlprefix, Controller* c) {
  String message = "<div class=\"row\"> <div class=\"column column-10\">";
  message += "Audiofile abspielen: ";
  message += "</div><div class=\"column column-90\">";

  String action = "send('" + urlprefix + "key=songid&value=' + (Number(document.getElementById('songid').value)))";

  message += "<form id=\"myformi2c\" >";
  message += "Song-Nummer: <input type=\"number\" min=\"0\" max=\"1000\" id=\"songid\" /><br><button type = \"button\" onclick=\"" + action + "\">Abspielen</button> ";
  message += "</form>";
  message += "</div>";
  message += "</div>";
  c->sendContent(message);


  File root = source->open("/");
  File file = root.openNextFile();
  while(file){
    c->sendContent(file.name());
    c->sendContent("<br>");
    file = root.openNextFile();
  }
}

#endif

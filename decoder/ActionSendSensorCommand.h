/*
 * ActionSendSensorCommand.h
 *
 *  Created on: 26.03.2020
 *      Author: Petr Osipov
 */

#ifndef ACTIONSENDSENSORCOMMAND_H_
#define ACTIONSENDSENSORCOMMAND_H_

#include "ISettings.h"
#include "Controller.h"
#include "ILoop.h"
#include <LinkedList.h>

// defines a structure for keeping the state of a sensor pin
struct SensorState{
  int gpio;
  int address;
  int value;
};


class ActionSendSensorCommand : public ISettings, public INotify, public ILoop {
public:
  ActionSendSensorCommand(Controller* c, int id, LinkedList<int> *list);
  virtual ~ActionSendSensorCommand();
  virtual void setSettings(String key, String value) override;
  void setSettings(SensorState status);
  void SensorCmd(int id, int direction, int source);
  virtual int loop();
private:
  Controller* controller;
  int16_t id;
  uint8_t richtung;
  LinkedList<SensorState> gpio;
};

#endif /* ACTIONSENDSENSORCOMMAND_H_ */

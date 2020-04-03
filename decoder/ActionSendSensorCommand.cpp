/*
   ActionSendSensorCommand.cpp
   This action is for reading sensor values (on/off) from such things like IR detectors, Track occupancy detectors, or similar items.
   It transmits the data to Z21 over WLAN (LAN_LOCONET_FROM_LAN), employing Loconet over LAN protocol.
   If your Z21 is connected to Rocrail, the sensors are visible as
   Command Station Z21
   Bus 1
   Address is "address"
   Value is 0/1

    Created on: 26.03.2020
        Author: Petr Osipov
*/

#include "ActionSendSensorCommand.h"
#include "Logger.h"
#include <LinkedList.h>
#include "GPIO.h"

ActionSendSensorCommand::ActionSendSensorCommand(Controller* c, int number, LinkedList<int> *list) {
  richtung = -1;
  controller = c;
  id = number;

  r = new requestInfo();
  r->art = requestInfo::ART::SENSOR;
  r->id = number;

  gpio = LinkedList<SensorState>();

  setModulName("Send Sensor Command");
  setConfigDescription("");

  int listSize = list->size();
  for (int h = 0; h < listSize; h++) {
    SensorState s;
    s.gpio = list->get(h);
    s.address = number + h;
    s.value = -1;
    gpio.add(s);
    GPIOobj.pinMode(s.gpio, INPUT, "GPIO for Sensor");
  }
}

ActionSendSensorCommand::~ActionSendSensorCommand() {
}



void ActionSendSensorCommand::setSettings(String key, String value) {

  int status = value.toInt();
  int pin = GPIOobj.string2gpio(key);

  int listSize = gpio.size();
  for (int h = 0; h < listSize; h++) {
    SensorState sensorstate = gpio.get(h);
    if (sensorstate.gpio == pin) {
      sensorstate.value = status;
      setSettings(sensorstate);
    }
  }
}

void ActionSendSensorCommand::SensorCmd(int toId, int direction, int source) {
  if (toId != id ) {
    return;
  }
}

void ActionSendSensorCommand::setSettings(SensorState status) {
  // TODO Move to Controller
  LinkedList<CmdSenderBase*>* list = controller->getSender();
  for (int i = 0; i < list->size(); i++) {
    CmdSenderBase* b = list->get(i);
    if (b == NULL) {
      Logger::log(LogLevel::ERROR, "ActionSendSensorCommand: Sender is null");
      continue;
    }
    b->sendSetSensor(status.address, status.value);
  }
}

/**
 * Polling of GBM exits. 
 */
int ActionSendSensorCommand::loop() {
  
  
  int listSize = gpio.size();
  for (int h = 0; h < listSize; h++) {

    SensorState sensorstate = gpio.get(h);

    if (sensorstate.gpio >= 0) {
      int value = GPIOobj.digitalRead(sensorstate.gpio);
      if (value != sensorstate.value) {
        sensorstate.value = value;
        setSettings(sensorstate);
        gpio.set(h, sensorstate);
      }
    }
  }
  return 100;
}


/*
 * Connectors.cpp
 *
 *  Created on: 18.07.2017
 *      Author: sven
 */

#include "Connectors.h"

#include "Connectors.h"
#include <Arduino.h>
#include "Logger.h"
Connectors::Connectors() {
}

Connectors::~Connectors() {
}




void Connectors::addAction(ISettings* a) {
    if (a == NULL) {
        Logger::log(LogLevel::ERROR, "Action is null");
    } else {
     actions.add(a);
    }
}



void Connectors::send(String key, String value) { 
    Logger::log(LogLevel::INFO, "PIPE", "Received (" + key + "/" + value + ")");
    for (int idx = 0; idx < actions.size(); idx++) {
        Logger::log(LogLevel::INFO, "PIPE", "Forward (" + key + "/" + value + ") to " + actions.get(idx)->getName());
        actions.get(idx)->setSettings(key, value);
    }
}

LinkedList<ISettings*>* Connectors::getActions() {
    return &actions;
}

String Connectors::createDebugDiagramm(String parent) {
    String out = getName() + "[label =\" " + getModulName() + "\\n" + getConfigDescription() + "\"];\r\n";
    for (int idx = 0; idx < actions.size(); idx++) {
        out = out + actions.get(idx)->createDebugDiagramm(getName());
    }
    return out;
}

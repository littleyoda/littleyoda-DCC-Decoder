/*
 * IFILTER.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "IFilter.h"
#include "Logger.h"

IFilter::IFilter() {
	// TODO Auto-generated constructor stub

}

IFilter::~IFilter() {
	// TODO Auto-generated destructor stub
}

void IFilter::addAction(ISettings* a) {
    actions.add(a);
}

void IFilter::setSettings(String key, String value) {
    send(key, value);
}


void IFilter::send(String key, String value) {
    Logger::log(LogLevel::TRACE, "Send: " + getName() + ": " + key + " -> " + value);
    for (int idx = 0; idx < actions.size(); idx++) {
        actions.get(idx)->setSettings(key, value);
    }
}


String IFilter::createDebugDiagramm(String parent) {
    String out = getName() + "[label =\" " + getModulName() + "\\n" + getName() + "\\n"+  getConfigDescription() + "\"];\r\n";
    if (!parent.equals("")) {
        out = out + parent + " -> " + getName() + ";\r\n";
    }
    for (int idx = 0; idx < actions.size(); idx++) {
        out = out + actions.get(idx)->createDebugDiagramm(getName());
    }
    return out;
}


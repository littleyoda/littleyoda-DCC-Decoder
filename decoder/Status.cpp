/*
 * STATUS.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "Status.h"
#include "Controller.h"
#include "INotify.h"

Status::Status(Controller* c) {
	controller = c;

}

Status::~Status() {
	// TODO Auto-generated destructor stub
}


void Status::setName(String s) {};
String Status::getName() {
	return "env";
}

String Status::getInternalStatus(String key) {
	String out = "";
	if (key == "locdata" || key == "*") {
		out += "locadata\n";
		Controller::Items* i = controller->getLocData();
		Controller::Items::iterator it = i->begin();
		while (it != i->end()) {
			int addr = it->first;
			LocData* l = it->second;
			out += String(addr) + ";D: " + String(l->direction) + ";S: " + String(l->speed) + ";F: " + String(l->status) + "\n";
			it++;
		}		
	}
	if (key == "requestlist" || key == "*") {
		out += "requestlist\n";
		LinkedList<INotify::requestInfo*>* l = controller->getRrequestList();
		for (int i =0; i < l->size(); i++) {
			INotify::requestInfo* d = l->get(i);
			if (d->art == d->LOCO) {
				out += "Lok ";
			} else {
				out += "Weiche ";
			}
			out += String(d->id);
			out += "\n";
		}
	}
	return out;
}

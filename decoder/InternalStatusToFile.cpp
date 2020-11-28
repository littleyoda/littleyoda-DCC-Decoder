/*
 * INTERNALSTATUSTOFILE.cpp
 *
 *  Created on: 07.06.2019
 *      Author: sven
 */

#include "InternalStatusToFile.h"

InternalStatusToFile::InternalStatusToFile() {
	// TODO Auto-generated constructor stub

}

InternalStatusToFile::~InternalStatusToFile() {
	// TODO Auto-generated destructor stub
}

void InternalStatusToFile::send(String modul, String key, String v) {
	file->print(modul + "|" + key + " => " + v + "\n");
}

void InternalStatusToFile::setFile(File* f) {
	file = f;
}
/*
 * json.h
 *
 *  Created on: 01.03.2018
 *      Author: sven
 */

#ifndef JSON_H_
#define JSON_H_

#include "Arduino.h"
#include "FS.h"
#include "jsmn.h"

class json {
public:
	json(File f);
	virtual ~json();
	void dump();
	String getValueByKey(int parentID, String key);
	String getValueByKey(int parentID, String key, String defaultValue);
	int getChildAt(int parentID, int idx);
	boolean getBooleanByKey(int parentID, String key);
	bool jsoneq(int pos, const char *s);
	String getString(int pos);
	int getIdxByKey(int parentID, String key);
	int getFirstChild(int parentID);
	bool keyExists(int parentID, String key);
	int getNextSiblings(int childID);
	void printElement(int id);
	int getFirstChildOfArrayByKey(int parentID, String key);
	int getNumberOfSiblings(int idx);
	bool isArray(int id);
	bool isValid();

private:
	boolean valid = false;
	char* buf;
	jsmn_parser* p;
	int elementCount;
	jsmntok_t* elements;
};

#endif /* JSON_H_ */

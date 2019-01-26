/*
 * json.cpp
 *
 *  Created on: 01.03.2018
 *      Author: sven
 */

#include "json.h"

json::json(File f) {
	elements = NULL;
	size_t size = f.size();
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Starting Parsing");
	buf = new char[size + 1];
	buf[size] = '\0';
	f.readBytes(buf, size);
	p = new jsmn_parser();
	jsmn_init(p);
	int r = jsmn_parse(p, buf, size, NULL, 0);
	if (r < 0) {
		printf("Failed to parse JSON (Token): %d\r\n", r);
		return;
	}

	elementCount = r;
	elements = new jsmntok_t[elementCount];
	jsmn_init(p);
	r = jsmn_parse(p, buf, size, elements, elementCount);
	if (r < 0) {
		Serial.printf("Failed to parse JSON: %d\r\n", r);
		return;
	}
	/* Assume the top-level element is an object */
	if (r < 1 || elements[0].type != JSMN_OBJECT) {
		printf("Object expected\r\n");
		return ;
	}

}

void json::dump() {
	for (int i = 1; i < elementCount; i++) {
		delay(0);
		printElement(i);
	}
}

void json::printElement(int i) {
	Serial.printf("[%3d] Start:%5d End:%5d J:%5d Parent:%5d Type: ", i, elements[i].start, elements[i].end, elements[i].skip, elements[i].parent);
	switch (elements[i].type) {
		case JSMN_UNDEFINED: Serial.print("?"); break;
		case JSMN_OBJECT: Serial.print("O");break;
		case JSMN_ARRAY: Serial.print("A");break;
		case JSMN_STRING: Serial.print("S");break;
		case JSMN_PRIMITIVE: Serial.print("P");break;
	}
	Serial.printf("\r\n");
}

json::~json() {
	Serial.println("MEM "  + String(ESP.getFreeHeap()) + " / Closing Parser");
	delete[] buf;
	delete p;
	delete[] elements;
}

int json::getIdxByKey(int parentID, String key) {
	int idx = getFirstChild(parentID);
	while (idx != -1) {
		if (jsoneq(idx, key.c_str())) {
			return idx;
		}
		idx = getNextSiblings(idx);
	}
	return -1;
}

int json::getFirstChild(int parentID) {
	if (parentID == -1) {
		return -1;
	}
	jsmntok_t* child = &elements[parentID + 1];

	// Check if a child is present
	if (child->parent == parentID) {
		return parentID + 1;
	}
	return -1;
}

int json::getNextSiblings(int childID) {
	if (childID == -1) {
		return -1;
	}
	jsmntok_t* child = &elements[childID];

	int idx = childID + child->skip;
	if (idx >= elementCount) {
		return -1;
	}
	jsmntok_t* child2 = &elements[idx];

	// Check if it is still the same parent
	if (child2->parent == child->parent) {
		return idx;
	} else {
		return -1;
	}
}

String json::getValueByKey(int parentID, String key) {
	int idx = getIdxByKey(parentID, key);
	if (idx == -1) {
		return "";
	}
	int cidx = getFirstChild(idx);
	if (cidx != -1) {
		return getString(cidx);
	}
	return "";
}


String json::getValueByKey(int parentID, String key, String defaultValue) {
	int idx = getIdxByKey(parentID, key);
	if (idx == -1) {
		return defaultValue;
	}
	int cidx = getFirstChild(idx);
	if (cidx != -1) {
		return getString(cidx);
	}
	return defaultValue;
}

String json::getString(int pos) {
	if (pos == -1) {
		return "ERROR";
	}
	jsmntok_t* tok = &elements[pos];
	String s;
	int len = tok->end - tok->start;
	int start = tok->start;

	// Remove " if necessary
	if (len > 2) {
		if (buf[start] == '\"' && buf[start + len - 1] == '\"') {
			start++;
			len = len - 2;
		}
	}

	// Copy Char to String (one by one)
	s.reserve(len);
	s = "";
	for (int i = 0; i < len; i++) {
		s.concat(buf[start + i]);
	}
	return s;
}

bool json::jsoneq(int pos, const char *s) {
	jsmntok_t* tok = &elements[pos];
	if (tok->type == JSMN_STRING  // Compare Typ
			&& ((int) strlen(s)) == (tok->end - tok->start) // Compare Length
			&& strncmp(buf + tok->start, s, (tok->end - tok->start)) == 0)  { // Compare String
		return true;
	}
	return false;
}

int json::getNumberOfSiblings(int idx) {
	int nr = 0;
	if (idx == -1) {
		return nr;
	}
	do {
		nr++;
		idx = getNextSiblings(idx);
	} while (idx != -1);
	return nr;
}

int json::getChildAt(int parentID, int idx) {
	int child = getFirstChild(parentID);
	for (int i = 0; i < idx; i++) {
		child = getNextSiblings(child);
	}
	return child;
}

int json::getFirstChildOfArrayByKey(int parentID, String key) {
	int idx = getIdxByKey(parentID, key);
	idx = getFirstChild(idx);
	if (idx == -1 || !isArray(idx)) {
		return -1;
	}
	idx = getFirstChild(idx);
	return idx;
}

bool json::isArray(int id) {
	if (id == -1) {
		return false;
	}
	return elements[id].type == JSMN_ARRAY;
}

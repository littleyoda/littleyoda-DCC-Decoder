/*
 * DataContainerSimpleList.h
 *
 *  Created on: 21.02.2018
 *      Author: sven
 */

#ifndef DATACONTAINERSIMPLELIST_H_
#define DATACONTAINERSIMPLELIST_H_

#include "Arduino.h"

template <typename T1, typename T2>
class DataContainerSimpleList {
public:
	DataContainerSimpleList(int initalSize, T1 a, T2 b) {
		keyNull = a;
		valueNull = b;
		containerSize = initalSize;
		containerUsed = 0;
		data = new container[initalSize];

	}

	struct container {
		T1 key;
		T2 value;
	};

	void put(T1 t1, T2 t2) {
		if (containerUsed == containerSize) {
			Serial.println("DataContainerSimpleList is full!");
			return;
		}
		for (int i = 0; i < containerUsed; i++) {
			if (data[i].key == t1) {
				data[i].value = t2;
				return;
			}
		}
		data[containerUsed].key = t1;
		data[containerUsed].value = t2;
		containerUsed++;
	};

	boolean containsKey(T1 t1) {
		for (int i = 0; i < containerUsed; i++) {
			container pm = data[i];
			if (pm.key == t1) {
//				last = &pm;
				return true;
			}
		}
		return false;
	}

	T1 getKey(int idx) {
		return data[idx].key;
	}
	T1 getValue(int idx) {
		return data[idx].value;
	}

	void removeByKey(T1 t1) {
		Serial.println("Not implemented! remove By key");
		return;
//		for (int i = 0; i < containerUsed(); i++) {
//			container* pm = data[i];
//			if (pm->key == t1) {
//				last = NULL;
//				mapping->remove(i);
//			}
//		}
	}
	boolean containsValue(T2 t2) {
		for (int i = 0; i < containerUsed; i++) {
			container pm = data[i];
			if (pm.value == t2) {
//				last = &pm;
				return true;
			}
		}
		return false;
	}

	T1 getKeyByValue(T2 t2) {
//		if (last != NULL && last->value == t2) {
//				return last->key;
//		}
		for (int i = 0; i < containerUsed; i++) {
			container pm = data[i];
			if (pm.value == t2) {
				return pm.key;
			}
		}
		return keyNull;
	}

	T2 getValueByKey(T1 t1) {
//		if (last != NULL && last->key == t1) {
//				return last->value;
//		}
		for (int i = 0; i < containerUsed; i++) {
			container pm = data[i];
			if (pm.key == t1) {
				return pm.value;
			}
		}
		return valueNull;
	}


	virtual ~DataContainerSimpleList() {

	}
	int used() {
		return containerUsed;
	}

	container* data;
	int containerSize;
	int containerUsed;
	T1 keyNull;
	T2 valueNull;
//	container* last = NULL;
};

#endif /* DATACONTAINERSIMPLELIST_H_ */

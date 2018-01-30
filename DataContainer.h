/*
 * DataContainer.h
 *
 *  Created on: 29.01.2018
 *      Author: sven
 */

#ifndef DATACONTAINER_H_
#define DATACONTAINER_H_

#include "LinkedList.h"

template <typename T1, typename T2>
class DataContainer {
public:
	DataContainer(T1 a, T2 b) {
		keyNull = a;
		valueNull = b;
		mapping = new LinkedList<container*>();
	}

	virtual ~DataContainer() {
		// TODO
	}

	void put(T1 t1, T2 t2) {
		container* p = new container();
		p->key = t1;
		p->value = t2;
		mapping->add(p);
	};

	boolean containsKey(T1 t1) {
		for (int i = 0; i < mapping->size(); i++) {
			container* pm = mapping->get(i);
			if (pm->key == t1) {
				last = pm;
				return true;
			}
		}
		return false;
	}

	boolean containsValue(T2 t2) {
		for (int i = 0; i < mapping->size(); i++) {
			container* pm = mapping->get(i);
			if (pm->value == t2) {
				last = pm;
				return true;
			}
		}
		return false;
	}

	T1 getKeyByValue(T2 t2) {
		if (last != NULL && last->value == t2) {
				return last->key;
		}
		for (int i = 0; i < mapping->size(); i++) {
			container* pm = mapping->get(i);
			if (pm->value == t2) {
				return pm->key;
			}
		}
		return keyNull;
	}

	T2 getValueByKey(T1 t1) {
		if (last != NULL && last->key == t1) {
				return last->value;
		}
		for (int i = 0; i < mapping->size(); i++) {
			container* pm = mapping->get(i);
			if (pm->key == t1) {
				return pm->value;
			}
		}
		return valueNull;
	}

	String toString(String sep) {
		String out = "";
		for (int i = 0; i < mapping->size(); i++) {
			container* pm = mapping->get(i);
			out += String(pm->key) + "/" + String(pm->value) + sep;
		}
		return out;
	}

private:
	struct container {
		T1 key;
		T2 value;
	};
	T1 keyNull;
	T2 valueNull;
	container* last = NULL;
	LinkedList<container*>* mapping;
};

#endif /* DATACONTAINER_H_ */

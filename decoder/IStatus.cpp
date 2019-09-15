#include "IStatus.h"

IStatus::IStatus() {
	id = NULL;
}

IStatus::~IStatus() {
	// TODO Auto-generated destructor stub
}

void IStatus::getInternalStatus(IInternalStatusCallback* cb, String key) {
}

void IStatus::setName(String s) {
	id = new String(s);
}

String IStatus::getName() {
	if (id == NULL) {
		return "";
	}
	return "" + (*id);
}

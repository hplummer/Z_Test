/*
 * BacnetValue.cpp
 *
 *  Created on: Aug 10, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include "BacnetUtils.h"
#include "BacnetValue.h"
#include "BacnetAppTypes.h"

namespace VIGBACNET {

void BacnetValue::clearModified() {
	_modified = false;
	_lastChange = 0;
}

void BacnetValue::valueModified() {
	_modified = true;
	_lastChange = time(0);
}

void BacnetValue::valueDirty() {
	_dirty = true;
	_lastDirty = time(0);
}

BacnetValue& BacnetValue::operator=(const BacnetValue &value) {
	if (this != &value) {
		set(value);
	}
	return *this;
}

std::string BacnetValue::toString() const {
	std::ostringstream os;
	out(os);
	return os.str();
}

BacnetErrorException BacnetValue::getErrorTypeException(const char *expectedType) const {
	std::ostringstream os;
	os << "type error: expected " << expectedType
	   << " but got " << typeName();
	return BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType, os.str());
}

}

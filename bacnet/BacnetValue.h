/*
 * BacnetValue.h
 *
 *  Created on: Aug 7, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetValue_h
#define BacnetValue_h

#include <typeinfo>
#include <cassert>
#include "fc.h"
#include "BacnetEnums.h"
#include "BacnetUtils.h"

namespace VIGBACNET {

class Encodable {
public:
	virtual ~Encodable() {};
	virtual int encode(uint8_t* buffer, size_t size) const = 0;
};

class BacnetValue;
typedef FC::Ref<BacnetValue> BacnetValueRef;

class BacnetValue : public FC::RefObject , public FC::Formatter {
public:
	BacnetValue() :
		_modified(false), _dirty(false), _lastChange(0), _lastDirty(0) {};
	virtual ~BacnetValue() {};

	virtual void resetLastChanged(time_t t = 0) { _lastChange = t; };
	virtual time_t lastChanged() const { return _lastChange; }
	virtual void clearModified();
	virtual bool isModified() const { return _modified; }
	virtual void resetLastDirty(time_t t = 0) { _lastChange = t; };
	virtual time_t lastDirty() const { return _lastDirty; }
	void clearDirty() { _dirty = false; }
	bool isDirty() const { return _dirty; }
	virtual const char* typeName() const { return DataTypeEnum::getName(type());}
	virtual const char* name() const { return typeName();}
	virtual const std::type_info& typeInfo() const { return typeid(*this); }
	std::string toString() const;
	BacnetValue& operator=(const BacnetValue&);

	virtual DataTypeEnum type() const = 0;
	virtual BacnetValueRef clone() const = 0;
	virtual bool set(const BacnetValue&, bool throwError=true) = 0;


protected:
	BacnetErrorException getErrorTypeException(const char *expectedType) const;
	void valueModified();
	void valueDirty();

	bool _modified;
	bool _dirty;
	time_t _lastChange;
	time_t _lastDirty;
};


} // BACNET namespace


#endif /* BACNETVALUE_H_ */

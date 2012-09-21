/*
 * BacnetServices.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetServices_h
#define BacnetServices_h

#include <limits>
#include "BacnetBaseTypes.h"

namespace VIGBACNET {

class ConfirmedRequest : public virtual FC::RefObject, public FC::Formatter {
public:
	virtual ~ConfirmedRequest() {};
	virtual ConfirmedServiceChoiceEnum service() const = 0;

};
typedef FC::Ref<ConfirmedRequest> ConfirmedRequestRef;

class ReadPropertyRequest : public ConfirmedRequest {
public:
	static const uint32_t NoIndex = 0xFFFFFFFFu;

	ReadPropertyRequest(const ObjectIdentifier& oid, const PropertyIdentifierEnum& pid,
			uint32_t idx = NoIndex) :
		_oid(oid), _pid(pid.get()), _index(idx) {}

	virtual ConfirmedServiceChoiceEnum service() const {
		return ConfirmedServiceChoiceEnum::ReadProperty;
	}

	virtual void out(std::ostream &os) const {
		os << "ReadPropertyRequest: {" <<
			   "oid: " << _oid << ", " <<
			  "propid: " << _pid << ", "
			  "array index: " << _index << "}";
	}

	ObjectIdentifier oid() const { return _oid; }
	PropertyIdentifierEnum pid() const { return _pid.get(); }
	Unsigned index() const { return _index; }

private:
	ObjectIdentifier _oid;
	PropertyIdentifier _pid;
	Unsigned _index;
};
typedef FC::Ref<ReadPropertyRequest> ReadPropertyRequestRef;

class WritePropertyRequest : public ConfirmedRequest {
public:
	static const uint32_t NoIndex = 0xFFFFFFFFu;

	WritePropertyRequest(const ObjectIdentifier& oid, const PropertyIdentifierEnum& pid,
			const BacnetValue& value = Null(), uint8_t priority = 0, uint32_t idx = NoIndex) :
		_oid(oid), _pid(pid.get()), _value(value.clone()), _priority(priority), _index(idx) {
	}

	WritePropertyRequest(const WritePropertyRequest& request) :
		_oid(request._oid), _pid(request._pid), _value(request._value->clone()),
		_priority(request._priority), _index(request._index) {
	}

	WritePropertyRequest& operator=(const WritePropertyRequest& request) {
		if (this != &request) {
			_oid = request._oid;
			_pid = request._pid;
			_value = request._value->clone();
			_priority = request._priority;
			_index = request._index;
		}
		return *this;
	}

	virtual ConfirmedServiceChoiceEnum service() const {
		return ConfirmedServiceChoiceEnum::WriteProperty;
	}

	virtual void out(std::ostream &os) const {
		os << "WritePropertyRequest: {" <<
			  "oid: " << _oid << ", " <<
			  "propid: " << _pid << ", "
			  "value: " << _value << ", " <<
			  "priority: " << _priority << ", " <<
			  "array index: " << _index << "}";
	}

	const BacnetValueRef value() const { return _value; }
	ObjectIdentifier oid() const { return _oid; }
	PropertyIdentifierEnum pid() const { return _pid.get(); }
	Unsigned index() const { return _index; }
	Unsigned priority() const { return _priority; }

private:
	ObjectIdentifier _oid;
	PropertyIdentifier _pid;
	BacnetValueRef _value;
	Unsigned _priority;
	Unsigned _index;
};
typedef FC::Ref<WritePropertyRequest> WritePropertyRequestRef;



}  // namespace VIGBACNET





#endif /* BACNETSERVICES_H_ */

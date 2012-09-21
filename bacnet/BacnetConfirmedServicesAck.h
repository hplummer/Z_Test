/*
 * BacnetServicesAck.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetServicesAck_h
#define BacnetServicesAck_h


#include <limits>
#include "BacnetBaseTypes.h"


namespace VIGBACNET {

class ConfirmedRequestAck : public virtual FC::RefObject, public FC::Formatter {

};
typedef FC::Ref<ConfirmedRequestAck> ConfirmedRequestAckRef;

class ReadPropertyAck : public ConfirmedRequestAck {
public:
	static const uint32_t NoIndex = 0xFFFFFFFFu;

	ReadPropertyAck(const ObjectIdentifier& oid, const PropertyIdentifierEnum& pid,
			const BacnetValue& value, uint32_t idx = NoIndex) :
		_oid(oid), _pid(pid.get()), _value(value.clone()), _index(idx) {
	}

	ReadPropertyAck(const ReadPropertyAck& ack) :
		_oid(ack._oid), _pid(ack._pid), _value(ack._value->clone()), _index(ack._index) {
	}

	ReadPropertyAck& operator=(const ReadPropertyAck& ack) {
		if (this != &ack) {
			_oid = ack._oid;
			_pid = ack._pid;
			_value = ack._value->clone();
			_index = ack._index;
		}
		return *this;
	}

	virtual void out(std::ostream &os) const {
		os << "ReadPropertyAck: {" <<
			  "oid: " << _oid << ", " <<
			  "propid: " << _pid << ", "
			  "value: " << _value << ", " <<
			  "array index: " << _index << "}";
	}

	ObjectIdentifier oid() const { return _oid; }
	PropertyIdentifierEnum pid() const { return _pid.get(); }
	Unsigned index() const { return _index; }
	const BacnetValueRef value() const { return _value; }

private:
	ObjectIdentifier _oid;
	PropertyIdentifier _pid;
	BacnetValueRef _value;
	Unsigned _index;
};
typedef FC::Ref<ReadPropertyAck> ReadPropertyAckRef;


class WritePropertyAck : public ConfirmedRequestAck {
public:
	WritePropertyAck(const ObjectIdentifier& oid, const PropertyIdentifierEnum& pid) :
		_oid(oid), _pid(pid.get()) {
	}

	WritePropertyAck(const WritePropertyAck& ack) :
		_oid(ack._oid), _pid(ack._pid) {
	}

	WritePropertyAck& operator=(const WritePropertyAck& ack) {
		if (this != &ack) {
			_oid = ack._oid;
			_pid = ack._pid;
		}
		return *this;
	}

	virtual void out(std::ostream &os) const {
		os << "WritePropertyAck: {" <<
			  "oid: " << _oid << ", " <<
			  "propid: " << _pid << "}";
	}

	ObjectIdentifier oid() const { return _oid; }
	PropertyIdentifierEnum pid() const { return _pid.get(); }

private:
	ObjectIdentifier _oid;
	PropertyIdentifier _pid;
};
typedef FC::Ref<WritePropertyAck> WritePropertyAckRef;

} // VIGBACNET


#endif /* BACNETSERVICESACK_H_ */

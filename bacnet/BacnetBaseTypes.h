/*
 * BacnetBaseTypes.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetBaseTypes_h
#define BacnetBaseTypes_h

#include <vector>
#include "BacnetUtils.h"
#include "BacnetAppTypes.h"
#include "BacnetValueGetterSetter.h"

namespace VIGBACNET {


/*
 * Missing type to complete Device, Ax, Bx
 * BACnetStatusFlags +
 * BACnetEventState +
 * BACnetEngineeringUnits +
 * BACnetPriorityArray
 * BACnetPolarity +
 * BACnetBinaryPV +
 * BACnetDeviceStatus +
 * BACnetServiceSupported +
 * BACnetObjectTypeSupported +
 * BACnetArray
 * ListOf
 */


template <typename T>
struct EnumBaseType : public virtual Enumerated, public virtual T {
	typedef typename T::Enum Enum;

	EnumBaseType() :
		Enumerated((uint32_t)T::getDefault()) {}

	EnumBaseType(const Enum& e) :
		Enumerated((uint32_t)e) {}

	EnumBaseType(const uint8_t* buffer, size_t size, size_t* numRead) :
		Enumerated(buffer, size, numRead) {}

	Enum get() const {
		return (Enum)_value;
	}

	void set(Enum e) {
		// Use Enumerated storage space
		Enumerated::set((uint32_t)e);
	}
	/**
	 * Set the value from a similar BacnetValue type or an Enumerated
	 *
	 * If both type are identical no check is done on the value, however if the base type
	 * is an Enumuerated, we make sure the value is compatible with the enum value for that
	 * specific type
	 *
	 * @param value the bacnet value to copy data from
	 * @param throwError flag to tell if the conversion should throw if cannot convert the value
	 *
	 * return true if successful conversion
	 */
	virtual bool set(const BacnetValue &value, bool throwError=true) {
		bool result = false;
		EnumBaseType *v = dynamic_cast<EnumBaseType*>(const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			valueDirty();
			result = true;
		} else {
			unsigned u;
			// try to convert to an enum value
			if (ValueGetter::get(value, u, false)) {
				if (std::string(getName((Enum)u)) != "Unknown") {
					Enumerated::set(u);
					result = true;
				} else if (throwError) {
					throwException(BacnetErrorException(ErrorClassEnum::Property,
							ErrorCodeEnum::ValueOutOfRange,
							FC::StringAPrintf("%d is not a valid value for type %s",u, typeName())));
				}
			} else if (throwError) {
				throwException(getErrorTypeException(value.typeName()));
			}
		}
		return result;
	}

	virtual const char* typeName() const {
		return T::getTypeName();
	}

	virtual const char* name() const {
		return getName((Enum)_value);
	}

	EnumBaseType& operator=(const EnumBaseType& val) {
		if (&val != this) {
			set(val.get()); // Avoid recursion call the BacnetValue.set(BacnetValue&)
		}
		return *this;
	}

	EnumBaseType& operator=(const Enum& e) {
		_value = (uint32_t)e;
		valueDirty();
		return *this;
	}

	Enum operator()() {
		return (Enum)_value;
	}

	bool operator==(const Enum& e) const {
		return _value == (uint32_t)e;
	}

	bool operator!=(const Enum& e) const {
		return _value != (uint32_t)e;
	}

	bool operator>(const Enum& e) const {
		return _value > (uint32_t)e;
	}

	bool operator>=(const Enum& e) const {
		return _value >= (uint32_t)e;
	}

	bool operator<(const Enum& e) const {
		return _value < (uint32_t)e;
	}

	bool operator<=(const Enum& e) const {
		return _value <= (uint32_t)e;
	}

	virtual void out(std::ostream &os) const {
		os << FC::StringAPrintf("{%s: %s}", typeName(), name());
	}

	virtual BacnetValueRef clone() const {
		return new EnumBaseType(*this);
	}
};


typedef EnumBaseType<DayOfTheWeekEnumDefinition> DayOfTheWeek;
typedef EnumBaseType<DataTypeEnumDefinition> DataType;
typedef EnumBaseType<ObjectTypeEnumDefinition> ObjectType;
typedef EnumBaseType<PropertyIdentifierEnumDefinition> PropertyIdentifier;
typedef EnumBaseType<DeviceStatusEnumDefinition> DeviceStatus;
typedef EnumBaseType<EventStateEnumDefinition> EventState;
typedef EnumBaseType<UnitsEnumDefinition> Units;
typedef EnumBaseType<PolarityEnumDefinition> Polarity;
typedef EnumBaseType<BinaryPVEnumDefinition> BinaryPV;
typedef EnumBaseType<SegmentationEnumDefinition> Segmentation;
typedef EnumBaseType<AbortReasonEnumDefinition> AbortReason;
typedef EnumBaseType<RejectReasonEnumDefinition> RejectReason;
typedef EnumBaseType<ErrorClassEnumDefinition> ErrorClass;
typedef EnumBaseType<ErrorCodeEnumDefinition> ErrorCode;


template<typename T>
class SequenceOf : public virtual BacnetValue {
public:
	SequenceOf() :
		_fixed(false) {}

	SequenceOf(size_t size, bool fixeSize = false, const T &val = T()) :
			_fixed(fixeSize) {
		for (size_t i = 0; i < size; i++) {
			_sequence.push_back(new T(val));
		}
	}

	SequenceOf(const SequenceOf &sequence) {
		*this = sequence;
	}

	SequenceOf& operator=(const SequenceOf &sequence) {
		if (this != &sequence) {
			_fixed = sequence._fixed;
			copyVec(sequence._sequence);
		}
		return *this;
	}

	bool isFixedSize() { return _fixed; }

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		SequenceOf *v = dynamic_cast<SequenceOf*>(
				const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			return true;
		} else if (throwError) {
			throwException(getErrorTypeException(value.typeName()));
		}
		return false;
	}

	FC::Ref<T> get(size_t idx) {
		return _sequence.at(idx);
	}

	void set(size_t idx, const FC::Ref<T> &val) {
		FC::Ref<T> ref = _sequence.at(idx);
		ref = val;
	}

	void push(FC::Ref<T> &val) {
		_sequence.push_back(val);
	}

	FC::Ref<T> pop() {
		FC::Ref<T> ref = _sequence.back();
		_sequence.pop_back();
		return ref;
	}

	size_t getCount() { return _sequence.size(); }

	virtual DataTypeEnum type() const {
		return DataTypeEnum::Unknown;
	}

	bool operator==(const SequenceOf &rh) const {
		return sameSequence(rh._sequence);
	}

	bool operator!=(const SequenceOf &rh) const {
		return !(*this == rh);
	}

	virtual BacnetValueRef clone() const {
		return new SequenceOf(*this);
	}

	virtual void out(std::ostream &os) const {
		os << "{SequenceOf: 0}";
	}

protected:
	typedef std::vector<FC::Ref<T> > SequenceVec;
	void copyVec(const SequenceVec& vec) {
		vec.clear();
		auto it = vec.begin();
		while (it != vec.end()) {
			FC::Ref<T> ref = new T(*it);
			_sequence.push_back(ref);
			it++;
		}
	}
	bool sameSequence(const SequenceVec &vec) const {
		if (vec.size() != _sequence.size()) {
			return false;
		}
		for (int i = 0; i < vec.size(); i++) {
			if (*(vec[i]) != *(_sequence[i])) {
				return false;
			}
		}
		return true;
	}

	SequenceVec _sequence;
	bool _fixed;
};

class Address : public virtual BacnetValue {
public:
	Address() :
		_net(0), _mac(0) {};
	Address(const Unsigned &num, const OctetString &mac) :
		_net(num), _mac(mac) {}
	Address(const Address &addr) :
		_net(addr._net),
		_mac(addr._mac) {}

	Address& operator=(const Address &val) {
		//Placeholder
		valueDirty();
		return *this;
	}

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		Address *v = dynamic_cast<Address*>(
				const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			valueDirty();
			return true;
		} else if (throwError) {
			throwException(getErrorTypeException(value.typeName()));
		}
		return false;
	}

	virtual DataTypeEnum type() const {
		return DataTypeEnum::Unknown;
	}

	bool operator==(const Address& rh) const {
		return _net == rh._net &&
			   _mac == rh._mac;
	}

	bool operator!=(const Address& rh) const {
		return !(*this == rh);
	}

	virtual BacnetValueRef clone() const {
		return new Address(*this);
	}

	virtual void out(std::ostream &os) const {
		os << "{Address: 0}";
	}

private:
	Unsigned _net;
	OctetString _mac;
};

class StatusFlags : public BitString {
public:
	enum {
		InAlarm,
		Fault,
		Overridden,
		OutOfService
	};

	StatusFlags(bool inAlarm = false, bool fault = false,
				bool overridden = false, bool outOfService = false) :
					BitString(4, false) {
		set(InAlarm, inAlarm);
		set(Fault, fault);
		set(Overridden, overridden);
		set(OutOfService, outOfService);
	}

	void inAlarm(bool active) {
		set(InAlarm, active);
	}
	bool inAlarm() const { return get(InAlarm); }

	void fault(bool active) {
		set(Fault, active);
	}
	bool fault() const { return get(Fault); }

	void overridden(bool active) {
		set(Overridden, active);
	}
	bool overridden() const { return get(Overridden); }

	void outOfService(bool active) {
		set(OutOfService, active);
	}
	bool outOfService() const { return get(OutOfService); }

	virtual BacnetValueRef clone() const {
		return new StatusFlags(*this);
	}

	virtual void out(std::ostream &os) const {
		os << FC::StringAPrintf(
				"{StatusFlag: Alarm: %s, Fault: %s, "
					"Overridden: %s, Out Of Service: %s",
					inAlarm() ? "true" : "false",
					fault() ? "true" : "false",
					overridden() ? "true" : "false",
					outOfService() ? "true" : "false");
	}

};

class ServicesSupported : public virtual BitString {
public:

	ServicesSupported() :
		BitString(40, false) {}

	void setSupported(const ServicesSupportedEnum& e, bool supported = false) {
		set(e, supported);
	}

	bool isSupported(const ServicesSupportedEnum& e) const {
		return get(e);
	}

	virtual const char* typeName() const { return "Services Supported"; }

	virtual BacnetValueRef clone() const {
		return new ServicesSupported(*this);
	}

};

class ObjectTypesSupported : public virtual BitString {
public:

	ObjectTypesSupported() :
		BitString(31, false) {}

	void setSupported(const ObjectTypeEnum& e, bool supported = false) {
		set(e, supported);
	}

	bool isSupported(const ObjectTypeEnum& e) const {
		return get(e);
	}

	virtual const char* typeName() const { return "Object Type Supported"; }

	virtual BacnetValueRef clone() const {
		return new ObjectTypesSupported(*this);
	}

};

class PriorityArray : public virtual BacnetValue {
public:

	virtual DataTypeEnum type() const {
		return DataTypeEnum::Unknown;
	}

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		PriorityArray *v = dynamic_cast<PriorityArray*>(
				const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			valueDirty();
			return true;
		} else if (throwError) {
			throwException(getErrorTypeException(value.typeName()));
		}
		return false;
	}

	virtual FC::Ref<BacnetValue> clone() const {
		return new PriorityArray(*this);
	}

	virtual void out(std::ostream &os) const {
		os << "{Priority Array: 0}";
	}

};

class DateTime : public virtual BacnetValue {
public:
	DateTime(Date today = Date::today(), Time now = Time::now()) :
		_date(today), _time(now) {
	}

	DateTime(const DateTime &dateTime) {
		_date = dateTime._date;
		_time = dateTime._time;
	}

	DateTime& operator=(const DateTime &val) {
		if (this != &val) {
			_date = val._date;
			_time = val._time;
		}
		return *this;
	}

	virtual DataTypeEnum type() const {
		return DataTypeEnum::Unknown;
	}

	virtual const char* typeName() const {
		return "DateTime";
	}

	Date getDate() const { return _date; }
	Time getTime() const { return _time; }

	bool operator==(const DateTime& rh) const {
		return _date == rh._date &&
			   _time == rh._time;
	}

	bool operator!=(const DateTime& rh) const {
		return !(*this == rh);
	}

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		DateTime *v = dynamic_cast<DateTime*>(
				const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			return true;
		} else if (throwError) {
			throwException(getErrorTypeException(value.typeName()));
		}
		return false;
	}

	virtual FC::Ref<BacnetValue> clone() const {
		return new DateTime(*this);
	}

	virtual void out(std::ostream &os) const {
		os << "{DateTime: ";
		os << _date << ", " << _time;
		os << "}";
	}

private:
	Date _date;
	Time _time;
};

class Error : public virtual BacnetValue {
public:
	Error(ErrorClassEnum eClass, ErrorCodeEnum eCode) :
		_eClass(eClass), _eCode(eCode) {}

	virtual DataTypeEnum type() const {
		return DataTypeEnum::Unknown;
	}

	virtual const char* typeName() const {
		return "Error";
	}

	ErrorClassEnum getClass() const { return _eClass; }
	ErrorCodeEnum getCode() const { return _eCode; }

	bool operator==(const Error& rh) const {
		return _eClass == rh._eClass &&
			   _eCode == rh._eCode;
	}

	bool operator!=(const Error& rh) const {
		return !(*this == rh);
	}

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		Error *v = dynamic_cast<Error*>(
				const_cast<BacnetValue*>(&value));
		if (v) {
			*this = *v;
			valueDirty();
			return true;
		} else if (throwError) {
			throwException(getErrorTypeException(value.typeName()));
		}
		return false;
	}

	virtual FC::Ref<BacnetValue> clone() const {
		return new Error(*this);
	}

	virtual void out(std::ostream &os) const {
		os << "{Error: ";
		os << _eClass << ", " << _eCode;
		os << "}";
	}

private:
	ErrorClassEnum _eClass;
	ErrorCodeEnum _eCode;
};


} //VIGBACNET
#endif /* BACNETBASETYPES_H_ */

/*
 * PrimitiveTypes.h
 *
 *  Created on: Aug 2, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef PrimitiveTypes_h
#define PrimitiveTypes_h

#include <cstdint>
#include <string>
#include <vector>

#include "fc.h"
#include "BacnetValue.h"

namespace VIGBACNET {

class EncodableValue : FC::Pointer {
public:
	virtual ~EncodableValue() { };
	virtual size_t encode(char* buffer, size_t size) const = 0;
	virtual FC::Ref<EncodableValue> clone() const = 0;
};


class PrimitiveValue : public virtual BacnetValue {
public:
	virtual ~PrimitiveValue() { };
	virtual size_t length() const = 0;
	virtual DataTypeEnum type() const = 0;
	virtual size_t encode(uint8_t*, size_t) const = 0;
};

class Null : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::Null;

	Null() { }
	Null(const uint8_t* buffer, size_t, size_t* numRead) {
		if (numRead) {
			*numRead = 0;
		}
	}
	Null& operator=(const Null& val) {
		valueDirty();
		return *this;
	}

	virtual size_t length() const { return 0; }
	virtual DataTypeEnum type() const { return TypeId; }
	int get() const { return 0; }

	virtual bool set(const BacnetValue &value, bool throwError=true);

	virtual size_t encode(uint8_t* buffer, size_t size) const {
		return 0;
	}

	bool operator==(const Null& rh) const {
		return true;
	}
	bool operator!=(const Null& rh) const {
		return false;
	}
	virtual BacnetValueRef clone() const {
		return new Null();
	}

	virtual void out(std::ostream &os) const {
		os << "{Null: 0}";
	}
};

template<typename T, DataTypeEnum::Enum ID>
class ScalarType : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = ID;

	ScalarType() :
		_value(0) {}
	ScalarType(const T& val) :
		_value(val) {}
	ScalarType(const ScalarType& val) :
		_value(val._value) {}
	ScalarType(const uint8_t* buffer, size_t size, size_t* numRead) {
		size_t readLen = length();
		if (readLen <= size) {
			::memcpy(&_value, buffer, readLen);
		} else {
			readLen = 0;
		}
		if (numRead) {
			*numRead = readLen;
		}
	}
	virtual ~ScalarType() {};

	ScalarType& operator=(const T& val) {
		set(val);
		return *this;
	}

	ScalarType& operator=(const ScalarType& val) {
		if (&val != this) {
			T v = val.get();
			set(v);
		}
		return *this;
	}

	virtual size_t length() const { return sizeof(T); }
	virtual DataTypeEnum type() const { return TypeId; }

	T get() const { return _value; }

	void set(const T& val) {
		if (!FC::eq(_value, val)) {
			_value = val;
			valueModified();
		}
		valueDirty();
	}

	virtual bool set(const BacnetValue &value, bool throwError=true) {
		ScalarType *v = dynamic_cast<ScalarType*>(
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

	virtual size_t encode(uint8_t* buffer, size_t size) const {
		size_t writeLen = length();
		if (writeLen <= size) {
			::memcpy(buffer, &_value, writeLen);
		} else {
			writeLen = 0;
		}
		return writeLen;
	}

	bool operator==(const ScalarType& rh) const {
		return FC::eq(_value, rh._value);
	}

	bool operator!=(const ScalarType& rh) const {
		return !(*this == rh);
	}

	operator T() { return _value; }

	virtual BacnetValueRef clone() const {
		return new ScalarType(*this);
	}

	virtual void out(std::ostream &os) const {
		os << FC::StringAPrintf("{%s: ", typeName());
		os << get();
		os << "}";
	}

protected:
	T _value;
};

typedef ScalarType<bool, DataTypeEnum::Boolean> Boolean;
typedef ScalarType<uint32_t, DataTypeEnum::Unsigned> Unsigned;
typedef ScalarType<uint32_t, DataTypeEnum::Enumerated> Enumerated;
typedef ScalarType<int32_t, DataTypeEnum::Integer> Integer;
typedef ScalarType<float, DataTypeEnum::Real> Real;
typedef ScalarType<double, DataTypeEnum::Double> Double;


class OctetString : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::OctetString;

	typedef uint8_t Octet;
	typedef std::vector<Octet> OctetBuffer;

	OctetString(const uint8_t * buffer, size_t length) {
		_buffer.insert(_buffer.begin(), buffer, &(buffer[length]));
	}

	OctetString(const OctetBuffer& buffer) :
		_buffer(buffer) { }

	OctetString(size_t size, Octet defaultVal=0) :
		_buffer(size, defaultVal) { }

	virtual ~OctetString() {}

	OctetString& operator=(const OctetString& val);

	virtual size_t length() const { return _buffer.size(); }
	virtual DataTypeEnum type() const { return TypeId; }

	const OctetBuffer& get() const { return _buffer; }
	void set(const OctetBuffer& buffer);

	void set(size_t idx, Octet val) {
		_buffer.at(idx) = val;
		valueDirty();
	}

	virtual bool set(const BacnetValue &value, bool throwError=true);

	Octet get(size_t idx) const {
		return _buffer.at(idx);
	}

	virtual size_t encode(uint8_t* buffer, size_t size) const;

	bool operator==(const OctetString& rh) const {
		return sameBuffer(rh._buffer);
	}

	bool operator!=(const OctetString& rh) const {
		return !(*this == rh);
	}

	virtual BacnetValueRef clone() const {
		return new OctetString(*this);
	}

	virtual void out(std::ostream &os) const;

protected:
	bool sameBuffer(const OctetBuffer& buffer) const;

	OctetBuffer _buffer;
};


class CharacterString : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::CharacterString;

	static bool supportedCharset(CharsetEnum charset) {
		return (charset == CharsetEnum::ANSI_X34 ||
				charset == CharsetEnum::ISO8859);
	}

	CharacterString() :
		_str(""), _charset(CharsetEnum::ANSI_X34) {}
	CharacterString(const uint8_t* buffer, size_t length,
			CharsetEnum charset = CharsetEnum::ANSI_X34);
	CharacterString(const std::string& str,
			CharsetEnum charset = CharsetEnum::ANSI_X34);
	CharacterString(const char* str,
			CharsetEnum charset = CharsetEnum::ANSI_X34);

	CharacterString& operator=(const CharacterString &value);

	virtual size_t length() const { return _str.length(); }
	virtual DataTypeEnum type() const { return TypeId; }

	const std::string& get() const { return _str; }
	void set(const std::string str);
	virtual bool set(const BacnetValue &value, bool throwError=true);
	CharsetEnum getCharset() const { return _charset; }

	virtual size_t encode(uint8_t* buffer, size_t size) const;

	bool operator==(const CharacterString& rh) const {
		return _str == rh._str;
	}

	bool operator!=(const CharacterString& rh) const {
		return !(*this == rh);
	}

	virtual BacnetValueRef clone() const {
		return new CharacterString(*this);
	}

	virtual void out(std::ostream &os) const;


protected:
	void validateCharset(CharsetEnum charset) const;

	std::string _str;
	CharsetEnum _charset;
};


class BitString : public PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::BitString;

	typedef std::vector<bool> BitBuffer;

	BitString(const uint8_t* buffer, size_t numBits) {
		decode(buffer, numBits);
	}
	BitString(const BitBuffer& buffer) :
		_buffer(buffer) { }
	BitString(size_t size = 0, bool defaultVal=false) :
		_buffer(size, defaultVal) {}

	virtual ~BitString() {}

	BitString& operator=(const BitString &value);

	virtual size_t length() const { return _buffer.size(); }
	virtual size_t byteLength() const {
		return (size_t)(std::ceil((double)_buffer.size() / 8.0));
	}
	virtual DataTypeEnum type() const { return TypeId; }
	const BitBuffer& get() const { return _buffer; }
	bool get(size_t idx) const {
		return _buffer.at(idx);
	}
	bool get(uint8_t* buffer, size_t size) const;
	void set(const BitBuffer& buffer);
	void set(size_t idx, bool val) {
		_buffer.at(idx) = val;
		valueDirty();
	}
	virtual bool set(const BacnetValue &value, bool throwError=true);
	virtual size_t encode(uint8_t* buffer, size_t size) const;
	bool operator==(const BitString& rh) const {
		return sameBuffer(rh._buffer);
	}
	bool operator!=(const BitString& rh) const {
		return !(*this == rh);
	}
	virtual BacnetValueRef clone() const {
		return new BitString(*this);
	}
	virtual void out(std::ostream &os) const;

protected:
	bool sameBuffer(const BitBuffer& buffer) const;
	void decode(const uint8_t* buffer, size_t numBits);


	BitBuffer _buffer;
};

class Date : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::Date;
	static const int Unspecified = 0XFF;

	static Date today();
	static Date todayUTC();
	static DayOfTheWeekEnum getDayOfTheWeek(int year, MonthEnum month, int day);
	static bool isLeapYear(uint16_t year);
	static uint8_t numDaysMonth(uint16_t year, MonthEnum month);

	Date(int year = Unspecified, MonthEnum month = MonthEnum::Unspecified_Month,
			   int day = Unspecified, DayOfTheWeekEnum wday = DayOfTheWeekEnum::Unspecified_Day) :
				   _year(year), _month(month),
				   _day(day), _wday(wday) {}
	Date(const uint8_t* buffer, size_t size, size_t* numRead);
	Date(const struct tm& today) {
		set(today);
	}

	Date& operator=(const Date &value);
	void set(const Date& date);
	void set(const struct tm& date);

	virtual bool set(const BacnetValue &value, bool throwError=true);
	int getYear() const { return _year;}
	void setYear(int year);
	bool isYearUnspecified() const { return _year == Unspecified; }

	MonthEnum getMonth() const { return _month; }
	void setMonth(MonthEnum month);
	bool isMonthUnspecified() const { return _month == MonthEnum::Unspecified_Month; }

	int getDay() const { return _day; }
	void setDay(int day);
	bool isDayUnspecified() const { return _day == Unspecified; }

	DayOfTheWeekEnum getWeekDay() const { return _wday; }
	void setWeekDay(DayOfTheWeekEnum wday);
	bool isWeekDayUnspecified() const { return _wday == DayOfTheWeekEnum::Unspecified_Day; }

	size_t length() const { return 4; }
	DataTypeEnum type() const { return TypeId; }
	size_t encode(uint8_t* buffer, size_t size) const;

	bool operator==(const Date& rh) const;
	bool operator!=(const Date& rh) const {
		return !(*this == rh);
	}
	BacnetValueRef clone() const {
		return new Date(*this);
	}

	virtual void out(std::ostream &os) const;

protected:
	size_t decode(const uint8_t* buffer, size_t size);

	int _year;
	MonthEnum _month;
	int _day;
	DayOfTheWeekEnum _wday;
};


class Time : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::Time;
	static const int Unspecified = 0XFF;

	static Time now();
	static Time nowUTC();
	static int getUtcOffset();
	static bool isDaylightSavingActive();

	Time(int  hour = Unspecified, int minute = Unspecified,
			   int second = Unspecified, int hundredths = Unspecified) :
				   _hour(hour), _minute(minute),
				   _second(second), _hundredths(hundredths) {}
	Time(const struct tm& now) {
		set(now);
	}
	Time(time_t now) {
		set(now);
	}
	Time(const uint8_t* buffer, size_t size, size_t* numRead);

	Time& operator=(const Time &value);

	void set(const Time& time);
	void set(const struct tm& now);
	void set(time_t now);

	virtual bool set(const BacnetValue &value, bool throwError=true);

	int getHour() const { return _hour;}
	void setHour(int hour);
	bool isHourUnspecified() const { return _hour == Unspecified; }

	int getMinute() const { return _minute; }
	void setMinute(int minute);
	bool isMinuteUnspecified() const { return _minute == Unspecified; }

	int getSecond() const { return _second; }
	void setSecond(int second);
	bool isSecondUnspecified() const { return _second == Unspecified; }

	int getHundredths() const { return _hundredths; }
	void setHundredths(int hundredths);
	bool isHundredthsUnspecified() const { return _hundredths == Unspecified; }

	size_t length() const { return 4; }
	DataTypeEnum type() const { return TypeId; }

	size_t encode(uint8_t* buffer, size_t size) const;

	bool operator==(const Time& rh) const;
	bool operator!=(const Time& rh) const {
		return !(*this == rh);
	}
	BacnetValueRef clone() const {
		return new Time(*this);
	}

	virtual void out(std::ostream &os) const;

protected:
	size_t decode(const uint8_t* buffer, size_t size);

	int _hour;
	int _minute;
	int _second;
	int _hundredths;
};

typedef uint32_t ObjectInstance;

class ObjectIdentifier : public virtual PrimitiveValue {
public:
	static const DataTypeEnum::Enum TypeId = DataTypeEnum::ObjectIdentifier;

	static const uint8_t InstanceBitLength = 22;
	static const ObjectInstance MaxInstance = (1 << InstanceBitLength) - 1;

	static void parseOid(uint32_t oid, ObjectTypeEnum& type,  uint32_t& instance);
	/**
	 * Make sure instance is within range
	 * BACnet instance are based on 22 bits value range
	 */
	static bool validInstance(ObjectInstance instance) {
		return (instance <= MaxInstance);
	}

	ObjectIdentifier() :
			_objType(ObjectTypeEnum::AnalogInput),
			_instance(MaxInstance) {}
	ObjectIdentifier(ObjectTypeEnum type, ObjectInstance instance);
	ObjectIdentifier(const ObjectIdentifier& oid);
	explicit ObjectIdentifier(const uint32_t& oid) {
		parseOid(oid, _objType, _instance);
	}

	ObjectIdentifier(const uint8_t* buffer, size_t size, size_t* numRead = 0);

	ObjectIdentifier& operator=(const ObjectIdentifier &value);

	size_t length() const { return 4; }
	DataTypeEnum type() const { return TypeId; }

	ObjectIdentifier get() const {
		return *this;
	}

	uint32_t getCoded() const { return ((uint32_t)_objType << InstanceBitLength) + _instance; }

	void set(const ObjectIdentifier &value);
	void set(uint32_t oid);
	virtual bool set(const BacnetValue &value, bool throwError=true);

	ObjectTypeEnum getType() const { return _objType; }
	void setType(ObjectTypeEnum type);

	ObjectInstance getInstance() const { return _instance; }
	void setInstance(ObjectInstance instance);

	virtual size_t encode(uint8_t* buffer, size_t size) const;

	bool operator==(const ObjectIdentifier& rh) const;

	bool operator!=(const ObjectIdentifier& rh) const {
		return !(*this == rh);
	}

	operator uint32_t() const { return getCoded(); }

	virtual BacnetValueRef clone() const {
		return new ObjectIdentifier(*this);
	}

	virtual void out(std::ostream &os) const;


protected:
	size_t decode(const uint8_t* buffer, size_t size);


	ObjectTypeEnum _objType;
	ObjectInstance _instance;
};


} // BACNET namespace

#endif /* PRIMITIVETYPES_H */

/*
 * BacnetAppTypes.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include "BacnetUtils.h"
#include "BacnetAppTypes.h"

namespace VIGBACNET {


bool Null::set(const BacnetValue &value, bool throwError) {
	Null *v = dynamic_cast<Null*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}


OctetString& OctetString::operator=(const OctetString& val) {
	if (&val != this) {
		set(val.get());
	}
	return  *this;
}

void OctetString::set(const OctetBuffer& buffer) {
	if (!sameBuffer(buffer)) {
		_buffer = buffer;
		valueModified();
	}
	valueDirty();
}

bool OctetString::set(const BacnetValue &value, bool throwError) {
	OctetString *v = dynamic_cast<OctetString*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}

 size_t OctetString::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = _buffer.size();
	if (writeLen <= size) {
		::memcpy(buffer, &_buffer[0], writeLen);
	} else {
		writeLen = 0;
	}
	return writeLen;
}

void OctetString::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: \"", typeName());
	size_t size = _buffer.size();
	for (size_t i = 0; i < size; i++) {
		os << FC::StringAPrintf("%02X", _buffer[i]);
		if (i < size - 1) {
			os << ", ";
		}
	}
	os << "\"}";
}

bool OctetString::sameBuffer(const OctetBuffer& buffer) const {
	if (_buffer.size() != buffer.size()) {
		return false;
	}
	for (size_t i = 0; i < _buffer.size(); i++) {
		if (_buffer[i] != buffer[i]) {
			return false;
		}
	}
	return true;
}

CharacterString::CharacterString(const uint8_t* buffer, size_t length,
		CharsetEnum charset) {
	_str.assign((const char*)buffer, length);
	validateCharset(charset);
	_charset = charset;
}

CharacterString::CharacterString(const std::string& str, CharsetEnum charset) {
	_str = str;
	validateCharset(charset);
	_charset = charset;
}

CharacterString::CharacterString(const char* str, CharsetEnum charset) {
	if (str == 0) {
		_str = "";
	} else {
		_str = str;
	}
	validateCharset(charset);
	_charset = charset;
}

CharacterString& CharacterString::operator=(const CharacterString &value) {
	if (&value != this) {
		_charset = value._charset;
		set(value.get());
	}
	return *this;
}

void CharacterString::set(const std::string str) {
	if (_str != str) {
		_str = str;
		valueModified();
	}
	valueDirty();
}

bool CharacterString::set(const BacnetValue &value, bool throwError) {
	CharacterString *v = dynamic_cast<CharacterString*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}


size_t CharacterString::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = _str.length();
	if (writeLen <= size) {
		_str.copy((char*)buffer, writeLen, 0);
	} else {
		writeLen = 0;
	}
	return writeLen;
}


void CharacterString::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: ", typeName());
	os << FC::CStrFormatter(_str);
	os << "}";
}

void CharacterString::validateCharset(CharsetEnum charset) const {
	if (!supportedCharset(charset)) {
		 throwException(BacnetErrorException(ErrorClassEnum::Property,
				 ErrorCodeEnum::CharacterSetNotSupported,
				 FC::StringAPrintf("Charset %s is not supported.",
						 CharsetEnum::getName(charset))));
	}
}


BitString& BitString::operator=(const BitString &value) {
	if (&value != this) {
		set(value.get());
	}
	return *this;
}

bool BitString::get(uint8_t* buffer, size_t size) const {
	FC_Debug(*this);
	if (byteLength() <= size) {
		const uint8_t InitMask = 0x80;
		uint8_t mask = InitMask;
		size_t bufIdx = 0;
		for (size_t i = 0; i < _buffer.size(); i++) {
			// If the mask is 0, time to switch to new buffer item
			// and reset mask to most significant bit.
			if (mask == 0) {
				mask = InitMask;
				bufIdx++;
			}
			// Do casting to get rid of compiler warning
			if (_buffer[i]) {
				buffer[bufIdx] = (uint8_t)(buffer[bufIdx] | mask);
			} else {
				buffer[bufIdx] = (uint8_t)(buffer[bufIdx] & ~mask);
			}
			mask = (uint8_t)(mask >> 1); // To get rid of conversion warning
		}
		return true;
	}
	return false;
}

void BitString::set(const BitBuffer& buffer) {
	if (!sameBuffer(buffer)) {
		_buffer = buffer;
		valueModified();
	}
	valueDirty();
}

bool BitString::set(const BacnetValue &value, bool throwError) {
	BitString *v = dynamic_cast<BitString*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}

size_t BitString::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = (size_t)(std::ceil((double)_buffer.size() / 8.0));
	if (writeLen <= size) {
		const uint8_t InitMask = 0x80;
		uint8_t mask = InitMask;
		int bufIdx = 0;
		for (size_t i = 0; i < _buffer.size(); i++) {
			// If the mask is 0, time to switch to new buffer item
			// and reset mask to most significant bit.
			if (mask == 0) {
				mask = InitMask;
				bufIdx++;
			}
			if (_buffer[i]) {
				buffer[bufIdx] |= mask;
			}
			mask = (uint8_t)(mask >> 1); // To get rid of conversion warning
		}
	} else {
		writeLen = 0;
	}
	return writeLen;
}


void BitString::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: \"", typeName());
	size_t size = _buffer.size();
	for (size_t i = 0; i < size; i++) {
		os << (_buffer[i] ? "true" : "false");
		if (i < size - 1) {
			os << ", ";
		}
	}
	os << "\"}";
}

bool BitString::sameBuffer(const BitBuffer& buffer) const {
	if (_buffer.size() != buffer.size()) {
		return false;
	}
	for (size_t i = 0; i < _buffer.size(); i++) {
		if (_buffer[i] != buffer[i]) {
			return false;
		}
	}
	return true;
}

void BitString::decode(const uint8_t* buffer, size_t numBits) {
	const uint8_t InitMask = 0x80;
	int mask = InitMask;
	int bufIdx = 0;
	for (size_t i = 0; i < _buffer.size(); i++) {
		// If the mask is 0, time to switch to new buffer item
		// and reset mask to most significant bit.
		if (mask == 0) {
			mask = InitMask;
			bufIdx++;
		}
		_buffer.push_back(buffer[bufIdx] & mask);
		mask >>= 1;
	}
}


Date::Date(const uint8_t* buffer, size_t size, size_t* numRead) {
	size_t readLen = decode(buffer, size);
	if (numRead) {
		*numRead = readLen;
	}
}

Date Date::today() {
	time_t ltime = ::time(0);
	struct tm today;
	::localtime_r(&ltime, &today);
	return Date(today);
}

Date Date::todayUTC() {
	time_t ltime = ::time(0);
	struct tm today;
	::gmtime_r(&ltime, &today);
	return Date(today);
}

DayOfTheWeekEnum Date::getDayOfTheWeek(int year, MonthEnum month, int day) {
	struct tm now;
	now.tm_year = year - 1900;
	now.tm_mon = month - 1;
	now.tm_mday = day;
	now.tm_isdst = -1; //assume local system for DST
	time_t ltime= ::mktime(&now);
	if (ltime < 0) {
		return DayOfTheWeekEnum::Unspecified_Day;
	} else {
		// Bacnet starts with Monday = 1, Sunday = 7
		return (now.tm_wday == 0 ? DayOfTheWeekEnum::Sunday :
					(DayOfTheWeekEnum::Enum)now.tm_wday);
	}
}

bool Date::isLeapYear(uint16_t year) {
	if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0)) {
		return true;
	}
	return false;
}

uint8_t Date::numDaysMonth(uint16_t year, MonthEnum month) {
	// Pad start with 0 to accommodate first month being index 1.
	uint8_t days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	// February
	if ((month == 2) && isLeapYear(year)) {
		return 29;
	} else if (month >= 1 && month <= 12) {
		return days[month - 1];
	} else {
		return 0;
	}
}

Date& Date::operator=(const Date &value) {
	if (&value != this) {
		set(value);
	}
	return *this;
}

void Date::set(const Date& date) {
	setYear(date._year);
	setMonth(date._month);
	setDay(date._day);
	setWeekDay((DayOfTheWeekEnum::Enum)date._wday);
	valueDirty();
}

void Date::set(const struct tm& date) {
	setYear(date.tm_year);
	setMonth((MonthEnum::Enum)(date.tm_mon + 1));
	setDay(date.tm_mday);
	// Bacnet expect week day 1 to 7 (Monday to Sunday)
	setWeekDay((date.tm_wday == 0 ? DayOfTheWeekEnum::Sunday :
			(DayOfTheWeekEnum::Enum)date.tm_wday));
	valueDirty();
}

bool Date::set(const BacnetValue &value, bool throwError) {
	Date *v = dynamic_cast<Date*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}

void Date::setYear(int year) {
	if (_year != year) {
		_year = year;
		valueModified();
	}
	valueDirty();
}

void Date::setMonth(MonthEnum month) {
	if (_month != month) {
		_month = month;
		valueModified();
	}
	valueDirty();
}
void Date::setDay(int day) {
	if (_day != day) {
		_day = day;
		valueModified();
	}
	valueDirty();
}

void Date::setWeekDay(DayOfTheWeekEnum wday) {
	if (_wday != wday) {
		_wday = wday;
		valueModified();
	}
	valueDirty();
}

size_t Date::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = length();
	if (length() <= size) {
		int i = 0;
		buffer[i++] = (uint8_t)(_year == Unspecified ? Unspecified : _year - 1900);
		buffer[i++] = (uint8_t)_month;
		buffer[i++] = (uint8_t)_day;
		buffer[i++] = (uint8_t)_wday;
	} else {
		writeLen = 0;
	}
	return writeLen;
}

bool Date::operator==(const Date& rh) const {
	return  (_year == rh._year) &&
			(_month == rh._month) &&
			(_day == rh._day) &&
			(_wday == rh._wday);
}



void Date::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: %s, %d(%s), %d}", typeName(),
			MonthEnum::getName(_month), _day,
			DayOfTheWeekEnum::getName(_wday), _year + 1900);
}

size_t Date::decode(const uint8_t* buffer, size_t size) {
	size_t readLen = length();
	if (readLen <= size) {
		int i = 0;
		_year = buffer[i++];
		_month = (MonthEnum::Enum)buffer[i++];
		_day = buffer[i++];
		_wday = (DayOfTheWeekEnum::Enum)buffer[i++];
	} else {
		readLen = 0;
	}
	return readLen;
}

Time::Time(const uint8_t* buffer, size_t size, size_t* numRead) {
	size_t readLen = decode(buffer, size);
	if (numRead) {
		*numRead = readLen;
	}
}

Time Time::now() {
	time_t ltime = ::time(0);
	struct tm today;
	::localtime_r(&ltime, &today);
	return Time(today);
}

Time Time::nowUTC() {
	time_t ltime = ::time(0);
	struct tm today;
	::gmtime_r(&ltime, &today);
	return Time(today);
}

int Time::getUtcOffset() {
	time_t ltime = ::time(0);
	struct tm today;
	::localtime_r(&ltime, &today);
	return (int)today.tm_gmtoff / 60;
}

bool Time::isDaylightSavingActive() {
	time_t ltime = ::time(0);
	struct tm today;
	::localtime_r(&ltime, &today);
	return (today.tm_isdst > 0);
}

Time& Time::operator=(const Time &value) {
	if (&value != this) {
		set(value);
	}
	return *this;
}

void Time::set(const Time& time) {
	setHour(time._hour);
	setMinute(time._minute);
	setSecond(time._second);
	setHundredths(time._hundredths);
	valueDirty();
}

void Time::set(const struct tm& now) {
	setHour(now.tm_hour);
	setMinute(now.tm_min);
	setSecond(now.tm_sec);
	setHundredths(0xFF); // Don't care
	valueDirty();
}

void Time::set(time_t now) {
	struct tm today;
	::localtime_r(&now, &today);
	set(today);
}

bool Time::set(const BacnetValue &value, bool throwError) {
	Time *v = dynamic_cast<Time*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}

void Time::setHour(int hour) {
	if (_hour != hour) {
		_hour = hour;
		valueModified();
	}
	valueDirty();
}

void Time::setMinute(int minute) {
	if (_minute != minute) {
		_minute = minute;
		valueModified();
	}
	valueDirty();
}

void Time::setSecond(int second) {
	if (_second != second) {
		_second = second;
		valueModified();
	}
	valueDirty();
}

void Time::setHundredths(int hundredths) {
	if (_hundredths != hundredths) {
		_hundredths = hundredths;
		valueModified();
	}
	valueDirty();
}

size_t Time::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = length();
	if (length() <= size) {
		int i = 0;
		buffer[i++] = (uint8_t)_hour;
		buffer[i++] = (uint8_t)_minute;
		buffer[i++] = (uint8_t)_second;
		buffer[i++] = (uint8_t)_hundredths;
	} else {
		writeLen = 0;
	}
	return writeLen;
}

bool Time::operator==(const Time& rh) const {
	return  (_hour == rh._hour) &&
			(_minute == rh._minute) &&
			(_second == rh._second) &&
			(_hundredths == rh._hundredths);
}

void Time::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: %d:%d:%d.%d}", typeName(),
			_hour, _minute, _second, _hundredths);
}

size_t Time::decode(const uint8_t* buffer, size_t size) {
	size_t readLen = length();
	if (readLen <= size) {
		int i = 0;
		_hour = buffer[i++];
		_minute = buffer[i++];
		_second = buffer[i++];
		_hundredths = buffer[i++];
	} else {
		readLen = 0;
	}
	return readLen;
}



ObjectIdentifier::ObjectIdentifier(ObjectTypeEnum type, uint32_t instance) :
	_objType(type), _instance(instance) {
	if (!validInstance(instance)) {
		 throwException(BacnetErrorException(ErrorClassEnum::Object,
				 ErrorCodeEnum::UnknownObject,
				 FC::StringAPrintf("Instance is too big (must be <%d.",
					MaxInstance).c_str()));
	}
}

ObjectIdentifier::ObjectIdentifier(const ObjectIdentifier& oid) {
	_objType = oid._objType;
	_instance = oid._instance;
}

ObjectIdentifier::ObjectIdentifier(const uint8_t* buffer, size_t size, size_t* numRead) {
	size_t readLen = decode(buffer, size);
	if (numRead) {
		*numRead = readLen;
	}
}


ObjectIdentifier& ObjectIdentifier::operator=(const ObjectIdentifier &value) {
	if (&value != this) {
		set(value);
	}
	return *this;
}

void ObjectIdentifier::set(const ObjectIdentifier &value) {
	setType(value._objType);
	setInstance(value._instance);
}

void ObjectIdentifier::set(uint32_t oid) {
	ObjectTypeEnum type;
	uint32_t instance;
	parseOid(oid, type, instance);
	setType(type);
	setInstance(instance);
}

bool ObjectIdentifier::set(const BacnetValue &value, bool throwError) {
	ObjectIdentifier *v = dynamic_cast<ObjectIdentifier*>(
			const_cast<BacnetValue*>(&value));
	if (v) {
		*this = *v;
		valueDirty();
		return true;
	} else if (throwError){
		throwException(getErrorTypeException(value.typeName()));
	}
	return false;
}

void ObjectIdentifier::setType(ObjectTypeEnum type) {
	if (_objType != type) {
		_objType = type;
		valueModified();
	}
	valueDirty();
}

void ObjectIdentifier::setInstance(uint32_t instance) {
	if (!validInstance(instance)) {
		throwException(BacnetErrorException(ErrorClassEnum::Property,
				 ErrorCodeEnum::ValueOutOfRange,
				 FC::StringAPrintf("Instance is too big (must be <%d.",
					MaxInstance).c_str()));
	}
	if (_instance != instance) {
		_instance = instance;
		valueModified();
	}
	valueDirty();
}

size_t ObjectIdentifier::encode(uint8_t* buffer, size_t size) const {
	size_t writeLen = length();
	if (writeLen <= size) {
		uint32_t oid = getCoded();
		::memcpy(buffer, &oid, writeLen);
	} else {
		writeLen = 0;
	}
	return writeLen;
}

bool ObjectIdentifier::operator==(const ObjectIdentifier& rh) const {
	return  (_objType == rh._objType) &&
			(_instance == rh._instance);
}

void ObjectIdentifier::out(std::ostream &os) const {
	os << FC::StringAPrintf("{%s: %s-%d}", typeName(), ObjectTypeEnum::getName(_objType),
				_instance);
}

void ObjectIdentifier::parseOid(uint32_t oid, ObjectTypeEnum& type,  uint32_t& instance) {
	type = (ObjectTypeEnum::Enum)(oid >> InstanceBitLength);
	instance = oid & MaxInstance;
}

size_t ObjectIdentifier::decode(const uint8_t* buffer, size_t size) {
	size_t readLen = length();
	if (readLen <= size) {
		uint32_t identifier;
		::memcpy(&identifier, buffer, readLen);
		_objType = (ObjectTypeEnum::Enum)(identifier >> InstanceBitLength);
		_instance = identifier & MaxInstance;
	} else {
		readLen = 0;
	}
	return readLen;
}


} // BACNET namespace




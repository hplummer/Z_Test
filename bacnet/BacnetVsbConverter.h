/*
 * BacnetVsbConverter.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetVsbConverter_h
#define BacnetVsbConverter_h

#include "BacnetServer.h"
#include "BacnetUtils.h"
#include "vsbhp.h"

namespace VIGBACNET {

struct VsbString {
	static const size_t MaxSize = 256;
	frString header;
	char str[MaxSize];
};

class VsbConverter {
public:
	/// Return a newly created pointer to VsbString populated with
	/// the character string
	/// Client is in charge of freeing memory
	static VsbString* toString(const CharacterString& bacStr) {
		VsbString* vsbStr = new VsbString();
		return toString(bacStr, *vsbStr);
	}

	/// Return passed VsbString populated with the character string
	static VsbString* toString(const CharacterString& bacStr, VsbString& vsbStr) {
		vsbStr.header.charset = (byte)bacStr.getCharset().get();
		vsbStr.header.codepage = 0; // Not supported at the moment
		// cast MaxSize to size_t to avoid compiler error
		vsbStr.header.len = (dword)std::min(bacStr.length(), (size_t)VsbString::MaxSize);
		std::string str = bacStr.get();
		for (size_t i = 0; i < vsbStr.header.len; i++) {
			vsbStr.str[i] = str[i];
		}
		return &vsbStr;
	}

	static CharacterString fromString(const frString& vsbStr) {
		CharacterString cStr;
		if (CharacterString::supportedCharset((CharsetEnum::Enum)vsbStr.charset)) {
			const frString* pstr = &vsbStr + 1;
			cStr = CharacterString((const uint8_t*)pstr, vsbStr.len,
					(CharsetEnum::Enum)vsbStr.charset);
		}
		return cStr;
	}

	// If the string is too long, truncate
	static void toObjectTypeSupported(const ObjectTypesSupported& bacObj,
						byte* vsbObj, size_t size) {
		// It too small need to truncate
		if (size < bacObj.byteLength()) {
			uint8_t *buf = new uint8_t[bacObj.byteLength()];
			bacObj.get(buf, bacObj.byteLength());
			::memcpy(vsbObj, buf, size);
		} else {
			bacObj.get(vsbObj, size);
		}
	}

	static void toDateTime(const DateTime& bacDT,
						frTimeDate& vsbDT) {
		Date date = bacDT.getDate();
		Time time = bacDT.getTime();

		vsbDT.hour = (byte)time.getHour();
		vsbDT.minute = (byte)time.getMinute();
		vsbDT.second = (byte)time.getSecond();
		vsbDT.hundredths = (byte)time.getHundredths();
		vsbDT.year = (word)(date.getYear() + 1900);
		vsbDT.month = (byte)date.getMonth();
		vsbDT.day = (byte)date.getDay();
		vsbDT.weekday = (byte)date.getWeekDay();
		vsbDT.dst = Time::isDaylightSavingActive();
		vsbDT.utcoffset = Time::getUtcOffset();
	}

	static void toTime(const Time& bacTime, frTime& vsbTime) {
		vsbTime.hour = (byte)bacTime.getHour();
		vsbTime.minute = (byte)bacTime.getMinute();
		vsbTime.second = (byte)bacTime.getSecond();
		vsbTime.hundredths = (byte)bacTime.getHundredths();
	}

	static void toDate(const Date& bacDate, frDate& vsbDate) {
		vsbDate.year = (byte)bacDate.getYear();
		vsbDate.month = (byte)bacDate.getMonth();
		vsbDate.day = (byte)bacDate.getDay();
		vsbDate.dow = (byte)bacDate.getWeekDay();
	}

	static uint16_t toError(const Error& error) {
		return (uint16_t)(((uint16_t)error.getClass() << 8) +
				((uint16_t)error.getCode()() & 0xFF));
	}

	static Error fromError(uint16_t err) {
		return Error((ErrorClassEnum::Enum)(err >> 8),
				(ErrorCodeEnum::Enum)(err & 0xFF));
	}

	static bool isSupportedDataType(const BacnetValue& val);

	static bool toVbag(const BacnetValue& val, frVbag& bag);
	static BacnetValueRef fromVbag(const frVbag& bag);

};




} // VIGBACNET


#endif /* BACNETVSBCONVERTER_H_ */

/*
 * BacnetValueGetterSetter.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetValueGetterSetter_h
#define BacnetValueGetterSetter_h

#include <type_traits>
#include "BacnetValue.h"
#include "BacnetUtils.h"

namespace VIGBACNET {

/**
 * Define a base template structure to identify primitive types (in, uint, float, double, etc)
 *
 * This template is the base to tell if the template element if of primitive type.  The value
 * is by default set to 0 for any non primitive element and the cast function do nothing.
 * The template is supposed to be specialized for all the primitive type and each cast functions
 * implemented to handle each type conversion properly.
 */
template<typename T>
struct IsPrimitiveType {
	enum { Value = 0 };
	/// Convert an element of type T from an element of type From
	template<typename From>
	static void castFrom(const From& from, T& to) {
	}
	/// Convert an element of type T to an element of type To
	template<typename To>
	static void castTo(const T& from, To& to) {
	}
};
template<>
struct IsPrimitiveType<bool> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, bool& to) {
		to = (bool)from;
	}
	template<typename To>
	static void castTo(const bool& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<int8_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, int8_t& to) {
			to = (int8_t)from;
	}
	template<typename To>
	static void castTo(const int8_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<int16_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, int16_t& to) {
			to = (int16_t)from;
	}
	template<typename To>
	static void castTo(const int16_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<int32_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, int32_t& to) {
			to = (int32_t)from;
	}
	template<typename To>
	static void castTo(const int32_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<int64_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, int64_t& to) {
			to = (int64_t)from;
	}
	template<typename To>
	static void castTo(const int64_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<uint8_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, uint8_t& to) {
			to = (uint8_t)from;
	}
	template<typename To>
	static void castTo(const uint8_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<uint16_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, uint16_t& to) {
			to = (uint16_t)from;
	}
	template<typename To>
	static void castTo(const uint16_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<uint32_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, uint32_t& to) {
			to = (uint32_t)from;
	}
	template<typename To>
	static void castTo(const uint32_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<uint64_t> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, uint64_t& to) {
			to = (uint64_t)from;
	}
	template<typename To>
	static void castTo(const uint64_t& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<float> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, float& to) {
			to = (float)from;
	}
	template<typename To>
	static void castTo(const float& from, To& to) {
		to = (To)from;
	}
};
template<>
struct IsPrimitiveType<double> {
	enum { Value = 1 };
	template<typename From>
	static void castFrom(const From& from, double& to) {
			to = (double)from;
	}
	template<typename To>
	static void castTo(const double& from, To& to) {
		to = (To)from;
	}
};
template<typename T>
struct IsString {
	enum { Value = 0 };
};
template<>
struct IsString<std::string> {
	enum { Value = 1 };
};

template <typename From, typename To>
void toString(const From& from, To& to) {
}

struct ValueGetter {
	static std::string lastError;

	static bool get(const BacnetValue&, bool&, bool throwError=true);
	static bool get(const BacnetValue&, int&, bool throwError=true);
	static bool get(const BacnetValue&, long&, bool throwError=true);
	static bool get(const BacnetValue&, unsigned&, bool throwError=true);
	static bool get(const BacnetValue&, unsigned long&, bool throwError=true);
	static bool get(const BacnetValue&, float&, bool throwError=true);
	static bool get(const BacnetValue&, double&, bool throwError=true);
	static bool get(const BacnetValue&, std::string&, bool throwError=true);
	static bool get(const BacnetValue& from, BacnetValue &to, bool throwError=true) {
		return to.set(from, throwError);
	}
	static bool cast(const BacnetValue& from, BacnetValue& to, bool throwError = true) {
		return get(from, to, throwError);
	}
	// Prevent this template function to be called for any BacnetValu derived type
	template <typename T>
	static typename std::enable_if<not std::is_base_of<BacnetValue, T>::value, bool>::type
	cast(const BacnetValue& from, T& to, bool throwError=true) {
		// If it is a primitive type look for a match
		if (IsPrimitiveType<T>::Value) {
			{
				bool v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				int v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				long v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				unsigned v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				unsigned long v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				float v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
			{
				double v;
				if (get(from, v, false)) {
					IsPrimitiveType<T>::castFrom(v, to);
					return true;
				}
			}
		}
		if (IsString<T>::Value) {
			if (get(from, to, false)) {
				return true;
			}
			toString(from, to);
			return true;
		}
		std::ostringstream oss;
		oss << "type error: expected " << FC::Demangler(typeid(to).name()) << " but got " <<
				FC::Demangler(typeid(from).name());
		lastError = oss.str();
		throwException(BacnetErrorException(ErrorClassEnum::Property,
				ErrorCodeEnum::InvalidDataType, oss.str()));
		return false;
	}
};

struct ValueSetter {
	static std::string lastError;

	static bool set(bool, BacnetValue&, bool throwError=true);
	static bool set(int, BacnetValue&, bool throwError=true);
	static bool set(unsigned, BacnetValue&, bool throwError=true);
	static bool set(float, BacnetValue&, bool throwError=true);
	static bool set(double, BacnetValue&, bool throwError=true);
	static bool set(const std::string&, BacnetValue&, bool throwError=true);
	static bool set(const char*, BacnetValue&, bool throwError=true);
	static bool set(const BacnetValue& from, BacnetValue &to, bool throwError=true) {
		return to.set(from, throwError);
	}
	static bool cast(const BacnetValue& from, BacnetValue& to, bool throwError = true) {
		return set(from, to, throwError);
	}
	// Prevent this template function to be called for any BacnetValu derived type
	template <typename T>
	static typename std::enable_if<not std::is_base_of<BacnetValue, T>::value, bool>::type
	cast(const T& from, BacnetValue& to, bool throwError=true) {
		// If it is a primitive type look for a match
		if (IsPrimitiveType<T>::Value) {
			{
				bool v;
				IsPrimitiveType<T>::castTo(from, v);
				if (set(v, to, false)) {
					return true;
				}
			}
			{
				int v;
				IsPrimitiveType<T>::castTo(from, v);
				if (set(v, to, false)) {
					return true;
				}
			}
			{
				unsigned v;
				IsPrimitiveType<T>::castTo(from, v);
				if (set(v, to, false)) {
					return true;
				}
			}
			{
				float v;
				IsPrimitiveType<T>::castTo(from, v);
				if (set(v, to, false)) {
					return true;
				}
			}
			{
				double v;
				IsPrimitiveType<T>::castTo(from, v);
				if (set(v, to, false)) {
					return true;
				}
			}
		}
		if (IsString<T>::Value) {
			if (set(from, to, false)) {
				return true;
			}
		}
		std::ostringstream oss;
		oss << "type error: expected " << FC::Demangler(typeid(to).name()) << " but got " <<
				FC::Demangler(typeid(from).name());
		lastError = oss.str();
		throwException(BacnetErrorException(ErrorClassEnum::Property,
				ErrorCodeEnum::InvalidDataType, oss.str()));
		return false;
	}
};

} // namespace VIGBACNET



#endif /* BACNETVALUEGETTERSETTER_H_ */

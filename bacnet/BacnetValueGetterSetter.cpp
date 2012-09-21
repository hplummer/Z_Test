/*
 * BacnetValueGetterSetter.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include "BacnetUtils.h"
#include "BacnetAppTypes.h"
#include "BacnetValueGetterSetter.h"

namespace VIGBACNET {

template <>
void toString<BacnetValue, std::string>(const BacnetValue& from, std::string& to) {
	std::ostringstream oss;
	oss << from;
	to = oss.str();
}

BacnetErrorException getErrorTypeExcepition(const char* from, const char *to) {
	std::ostringstream os;
	os << "type error: expected " << to
	   << " but got " << from;
	return BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType, os.str());
}

std::string ValueGetter::lastError = "";
std::string ValueSetter::lastError = "";

bool ValueGetter::get(const BacnetValue& from, bool &to, bool throwError) {
	if (from.type() == DataTypeEnum::Boolean) {
		Boolean* v = dynamic_cast<Boolean*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "bool");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, int &to, bool throwError) {
	if (from.type() == DataTypeEnum::Integer) {
		Integer* v = dynamic_cast<Integer*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "int");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, long &to, bool throwError) {
	int i;
	if (get(from, i, throwError)) {
		to = i;
		return true;
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, unsigned &to, bool throwError) {
	if (from.type() == DataTypeEnum::Unsigned) {
		Unsigned* v = dynamic_cast<Unsigned*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else if (from.type() == DataTypeEnum::Enumerated) {
		Enumerated* v = dynamic_cast<Enumerated*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "unsigned");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, unsigned long &to, bool throwError) {
	int i;
	if (get(from, i, throwError)) {
		to = i;
		return true;
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, float& to, bool throwError) {
	if (from.type() == DataTypeEnum::Real) {
		Real* v = dynamic_cast<Real*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "float");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, double& to, bool throwError) {
	if (from.type() == DataTypeEnum::Double) {
		Double* v = dynamic_cast<Double*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else if (from.type() == DataTypeEnum::Real) {
		Real* v = dynamic_cast<Real*>(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "double");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueGetter::get(const BacnetValue& from, std::string& to, bool throwError) {
	if (from.type() == DataTypeEnum::CharacterString) {
		CharacterString* v = dynamic_cast<CharacterString*>
								(const_cast<BacnetValue*>(&from));
		to = v->get();
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition(from.typeName(), "string");
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(bool from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::Boolean) {
		Boolean* v = dynamic_cast<Boolean*>(&to);
		v->set(from);
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition("bool", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(int from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::Integer) {
		Integer* v = dynamic_cast<Integer*>(&to);
		v->set(from);
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition("int", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(unsigned from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::Unsigned) {
		Unsigned* v = dynamic_cast<Unsigned*>(&to);
		v->set(from);
		return true;
	} else if (to.type() == DataTypeEnum::Enumerated) {
		Enumerated* v = dynamic_cast<Enumerated*>((&to));
		v->set(from);
		return true;
	}else {
		BacnetErrorException ex = getErrorTypeExcepition("unsigned", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(float from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::Real) {
		Real* v = dynamic_cast<Real*>(&to);
		v->set(from);
		return true;
	} else if (to.type() == DataTypeEnum::Double) {
		Double* v = dynamic_cast<Double*>(&to);
		v->set(from);
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition("float", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(double from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::Double) {
		Double* v = dynamic_cast<Double*>(&to);
		v->set(from);
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition("double", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(const std::string& from, BacnetValue& to, bool throwError) {
	if (to.type() == DataTypeEnum::CharacterString) {
		CharacterString* v = dynamic_cast<CharacterString*>(&to);
		v->set(from);
		return true;
	} else {
		BacnetErrorException ex = getErrorTypeExcepition("string", to.typeName());
		lastError = ex.what();
		if (throwError) {
			throwException(ex);
		}
	}
	return false;
}

bool ValueSetter::set(const char *from, BacnetValue& to, bool throwError) {
	return set(std::string(from), to, throwError);
}

}  // namespace VIGBACNET



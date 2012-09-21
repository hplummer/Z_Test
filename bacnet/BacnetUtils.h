/*
 * BacnetUtils.h
 *
 *  Created on: Aug 10, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetUtils_h
#define BacnetUtils_h

#include "BacnetExceptions.h"

namespace VIGBACNET {

template <typename EX>
void throwException(const EX &ex) {
	FC_Debug1f("BACnet Stack Exception: '%s'", ex.what());
	throw ex;
}

template<typename To, typename From>
To value_cast(From* from, bool throwError=true) {
	To to = dynamic_cast<To>(from);
	if (!to && throwError) {
		std::ostringstream os;
		os << "type error: expected " << FC::Demangler(typeid(From).name())
		   << " but got " << FC::Demangler(typeid(From).name());
		throwException(BacnetApplicationException(os.str()));
	}
	return to;
}



} // VIGBACNET

#endif /* BACNETUTILS_H_ */

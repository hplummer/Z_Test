/*
 * BacnetExceptions.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetExceptions_h
#define BacnetExceptions_h

#include "BacnetEnums.h"

namespace VIGBACNET {

class BacnetException : public FC::Exception {
public:
	BacnetException(const std::string &what)
		: Exception(what) {}
	BacnetException(const BacnetException &copy)
		: Exception(copy._what) {}
};

class BacnetApplicationException : public BacnetException {
public:
	BacnetApplicationException(const std::string &what)
		: BacnetException(what) {}
	BacnetApplicationException(const BacnetApplicationException &copy)
		: BacnetException(copy._what) {}
};

/**
 * Type exception.
 */
class BacnetErrorException : public BacnetException {
public:
	BacnetErrorException(ErrorClassEnum eClass = ErrorClassEnum::getDefault(),
			ErrorCodeEnum eCode = ErrorCodeEnum::getDefault(),
			const std::string &what = "")
		: BacnetException(what), _eClass(eClass), _eCode(eCode) {}

	BacnetErrorException(const BacnetErrorException &copy)
		: BacnetException(copy._what), _eClass(copy._eClass), _eCode(copy._eCode) {}

	~BacnetErrorException() throw() {}

	virtual const char* what() const throw() {
		return FC::StringAPrintf("Error <%s:%s>: %s", _eClass.name(),
				_eCode.name(), _what.c_str()).c_str();
	}

	ErrorClassEnum eClass() const { return _eClass; }
	ErrorCodeEnum eCode() const { return _eCode; }

protected:
	ErrorClassEnum _eClass;
	ErrorCodeEnum _eCode;
};

} // VIGBACNET

#endif /* BACNETEXCEPTIONS_H_ */

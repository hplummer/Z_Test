/*
 * BacnetUnconfirmedRequest.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetUnconfirmedRequest_h
#define BacnetUnconfirmedRequest_h

#include <limits>
#include "BacnetBaseTypes.h"

namespace VIGBACNET {


class UnconfirmedRequest : public virtual FC::RefObject, public FC::Formatter {
public:
	virtual ~UnconfirmedRequest() {};
	virtual UnconfirmedServiceChoiceEnum service() const = 0;

};

class WhoIsRequest : public UnconfirmedRequest {
public:
	static const ObjectInstance MaxInstance = ObjectIdentifier::MaxInstance;

	WhoIsRequest(ObjectInstance min = 1, ObjectInstance max = MaxInstance) :
		_min(min), _max(max) {}

	Unsigned min() const { return _min; }
	Unsigned max() const { return _max; }

	virtual UnconfirmedServiceChoiceEnum service() const {
		return UnconfirmedServiceChoiceEnum::WhoIs;
	}

	virtual void out(std::ostream &os) const {
		os << "WhoIsRequest: {" <<
			  "Min Instance: " << _min << ", " <<
			  "Max Instance: " << _max << "}";
	}
private:
	Unsigned _min;
	Unsigned _max;
};

class IAmRequest : public UnconfirmedRequest {
public:
	IAmRequest(const ObjectInstance& oid, const Unsigned& maxApduLength,
			const Segmentation& segSupported, const Unsigned& vendorId) :
		_oid(oid), _maxApduLength(maxApduLength), _segSupported(segSupported), _vendorId(vendorId) {
	}

	ObjectIdentifier oid() const { return _oid; }
	Unsigned maxApduLength() const { return _maxApduLength; }
	Segmentation segmentationSupported() const { return _segSupported; }
	Unsigned vendorId() const { return _vendorId; }

	virtual UnconfirmedServiceChoiceEnum service() const {
		return UnconfirmedServiceChoiceEnum::IAm;
	}

	virtual void out(std::ostream &os) const {
		os << "IAmRequest: {" <<
			  "oid: " << _oid << ", " <<
			  "Max APDU Length: " << _maxApduLength << ", "
			  "Segmentation Supported: " << _segSupported <<
			  "Vendor Id: " << _vendorId << "}";
	}
private:
	ObjectIdentifier _oid;
	Unsigned _maxApduLength;
	Segmentation _segSupported;
	Unsigned _vendorId;
};

typedef FC::Ref<UnconfirmedRequest> UnconfirmedRequestRef;

}  // namespace VIGBACNET


#endif /* BACNETUNCONFIRMEDREQUEST_H_ */

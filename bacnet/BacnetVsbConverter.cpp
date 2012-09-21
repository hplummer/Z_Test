/*
 * BacnetVsbConverter.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include <typeinfo>
#include "BacnetVsbConverter.h"
#include "BacnetAppTypes.h"

namespace VIGBACNET {

struct DataTypeTransform {
	AppDatatypes vsbType;
	FC::Ref<BacnetValue> val;

	bool operator()(const DataTypeTransform& lh, const DataTypeTransform& rh) {
		return lh.vsbType < rh.vsbType;
	}
};

typedef std::set<DataTypeTransform, DataTypeTransform> TypeTransformSet;
typedef std::map<std::type_info, AppDatatypes> BacToVsbType;

TypeTransformSet typeSets = {
		{adtNull, new Null()},
		{adtBoolean, new Boolean()},
		{adtUnsigned, new Unsigned()},
		{adtInteger, new Integer()},
		{adtReal, new Real()},
		{adtDouble, new Double()},
		{adtOctetString, new OctetString(0)},
		{adtCharString, new CharacterString()},
		{adtBitString, new BitString()},
		{adtEnumerated, new Enumerated()},
		{adtDate, new Date()},
		{adtTime, new Time()},
		{adtObjectID, new ObjectIdentifier()},
		{adtObjectList, 0},
		{adtEmptyList, 0},
		{adtDateTimeStamp, 0},
		{adtPropRef, 0},
		{adtOptPropRef, 0},
		{adtSACK, 0},
		{adtError, new Error(ErrorClassEnum::Object, ErrorCodeEnum::UnknownObject)}
};

bool VsbConverter::isSupportedDataType(const BacnetValue& val) {
	DataTypeTransform s = {(AppDatatypes)val.type().get(), 0};
	auto it = typeSets.find(s);
	return ((it != typeSets.end()) && it->val);
}

bool VsbConverter::toVbag(const BacnetValue& val, frVbag& bag) {
	bool result = false;
	memset(&bag, 0, sizeof(frVbag));
	switch ((AppDatatypes)val.type().get()) {
	case adtNull:
	case adtBoolean:
	case adtUnsigned:
	case adtInteger:
	case adtReal:
	case adtDouble:
	case adtDate:
	case adtTime:
	case adtEnumerated:
	case adtObjectID:
		{
			const PrimitiveValue* prim = dynamic_cast<const PrimitiveValue*>(&val);
			bag.pdtype = val.type();
			prim->encode((uint8_t*)&bag.pd, prim->length());
			result =  true;
		}
		break;
	case adtOctetString:
	case adtBitString:
		{
			const PrimitiveValue* prim = dynamic_cast<const PrimitiveValue*>(&val);
			bag.pdtype = val.type();
			bag.pd.uval = (uint32_t)prim->length();
			prim->encode((uint8_t*)&bag.ps, prim->length());
			result = true;
		}
		break;
	case adtCharString:
		{
			const CharacterString *v = value_cast<const CharacterString*>(&val);
			bag.pdtype = val.type();
			bag.pd.stval.charset = v->getCharset();
			bag.pd.stval.len = (uint32_t)v->length();
			v->encode(bag.ps.psval, sizeof(bag.ps.psval));
			result = true;
		}
		break;
	default:
		result = false;
		break;
	}
	if (!result) {
		const Error *v = dynamic_cast<const Error*>(&val);
		if (v) {
			bag.pdtype = adtError;
			bag.pd.errval = toError(*v);
			result = true;
		}
	}
	return result;
}

BacnetValueRef VsbConverter::fromVbag(const frVbag& bag) {
	BacnetValueRef ref;
	switch (bag.pdtype) {
	case adtNull:
		ref = new Null();
		break;
	case adtBoolean:
		ref = new Boolean(bag.pd.uval);
		break;
	case adtUnsigned:
		ref = new Unsigned(bag.pd.uval);
		break;
	case adtInteger:
		ref = new Integer((int)bag.pd.sval);
		break;
	case adtReal:
		ref = new Real(bag.pd.fval);
		break;
	case adtDouble:
		ref = new Double(bag.pd.dval);
		break;
	case adtDate:
		ref = new Date((uint8_t*)&(bag.pd.dtval), sizeof(bag.pd), 0);
		break;
	case adtTime:
		ref = new Time((uint8_t*)&(bag.pd.tval), sizeof(bag.pd), 0);
		break;
	case adtEnumerated:
		ref = new Enumerated(bag.pd.uval);
		break;
	case adtObjectID:
		ref = new ObjectIdentifier(bag.pd.uval);
		break;
	case adtOctetString:
		ref = new OctetString((uint8_t*)&(bag.ps), bag.pd.uval);
		break;
	case adtBitString:
		ref = new BitString((uint8_t*)&(bag.ps), bag.pd.uval);
		break;
	case adtCharString:
		ref = new CharacterString((uint8_t*)&(bag.ps), bag.pd.stval.len);
		break;
	case adtError:
		ref = new Error(ErrorClassEnum::Enum((bag.pd.errval >> 8) & 0xFF),
						ErrorCodeEnum::Enum(bag.pd.errval & 0xFF));
		break;
	}
	return ref;
}

} // VIGBACNET

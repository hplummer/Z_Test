/*
 * BacnetProperties.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include <cassert>
#include "BacnetProperties.h"
#include "BacnetExceptions.h"

namespace VIGBACNET {


Property& Property::operator=(const Property& prop) {
	if (this != &prop) {
		copy(prop, true);
	}
	return *this;
}

void Property::copy(const Property& prop, bool checkType) {
	if (checkType) {
		assert(_value);
		if ((_value->type() != prop._value->type())) {
			throwException(BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType,
					"Incompatible type."));
		}
	}
	_isRequired = prop._isRequired;
	_isRemoteWrittable = prop._isRemoteWrittable;
	if (prop._value) {
		_value = prop._value->clone();
	}
}


ObjectProperty& ObjectProperty::operator=(const ObjectProperty& prop) {
	if (this != &prop) {
		copy(prop, true);
	}
	return *this;
}

void ObjectProperty::copy(const ObjectProperty& prop, bool checkType) {
	if (checkType) {
		assert(_defaultProp);
		assert(_defaultProp->getValue());
		if ((_defaultProp->getValue()->type() !=
			  prop.getDefaultProperty()->getValue()->type())) {
			throwException(BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType,
					"Incompatible type."));
		}
	}
	_objType = prop._objType;
	_propId = prop._propId;
	_defaultProp = new Property(*(prop._defaultProp));
}

// Local namespace
namespace {

typedef std::map<ObjectTypeEnum, ObjectPropertySet> ObjectPropertiesMap;

/**
 * Get the set of object properties
 * The Set is build upon the first request and contains all the
 * know properties for each known object.
 * The set is built dynamically using an include file with a specific format
 * (see ObjectPropertiesDefinition.i)
 */
static const ObjectPropertiesMap& getObjectPropertiesMap() {
	static bool initialized = false;
	static ObjectPropertiesMap objPropertiesMap;

	if (!initialized) {
		#define START_OBJECT(typeEnum, supported) 												\
		{ 																						\
			ObjectTypeEnum objType = (typeEnum);												\
			ObjectPropertySet properties(ObjectPropertiesComparator::sortByObjAndPropAsc);		\
			int count = 0;
		#define PROPERTY(propId, required, writtable, bacValueRef) 								\
			properties.insert(new ObjectProperty(objType, (propId), 							\
								new Property(bacValueRef, (required), (writtable)))); 			\
			count++;
		#define END_OBJECT																		\
			FC_Debug1f("Created %d properties for Bacnet Object %s", 							\
									count, ObjectTypeEnum::getName(objType)); 					\
			objPropertiesMap[objType] = properties;												\
			count = 0;																			\
		}
		#include "ObjectPropertiesDefinition.i"
		#undef START_OBJECT
		#undef PROPERTY
		#undef END_OBJECT

		initialized = true;
	}
	return objPropertiesMap;
}

} // local namespace

void ObjectProperties::getAll(ObjectPropertySet& s) {
	const auto objProps = getObjectPropertiesMap();
	for (auto itObj = objProps.begin(); itObj != objProps.end(); itObj++) {
		for (auto itProp = itObj->second.begin(); itProp != itObj->second.end(); itProp++) {
			s.insert(new ObjectProperty(**itProp));
		}
	}
}

void ObjectProperties::getAll(ObjectTypeEnum type, ObjectPropertySet& s) {
	const auto objProps = getObjectPropertiesMap();
	auto itObj = objProps.find(type);
	if (itObj != objProps.end()) {
		for (auto itProp = itObj->second.begin(); itProp != itObj->second.end(); itProp++) {
			s.insert(new ObjectProperty(**itProp));
		}
	}
}

ObjectPropertyRef ObjectProperties::get(ObjectTypeEnum type, PropertyIdentifierEnum id) {
	const auto objProps = getObjectPropertiesMap();
	auto itObj = objProps.find(type);
	if (itObj != objProps.end()) {
		ObjectPropertyRef refCmp;
		refCmp = new ObjectProperty(type, id, 0);
		auto itProp = itObj->second.find(refCmp);
		if (itProp != itObj->second.end()) {
			return new ObjectProperty(**itProp);
		}
	}
	return 0;
}

void ObjectProperties::getEssential(ObjectTypeEnum type, ObjectPropertySet& s) {
	ObjectPropertyRef ref;
	// ObjectId must exist or we do not support that object at all
	if ((ref = get(type, PropertyIdentifierEnum::ObjectIdentifier))) {
		s.insert(ref);
	} else {
		return;
	}
	if ((ref = get(type, PropertyIdentifierEnum::ObjectType))) {
		s.insert(ref);
	}
	if ((ref = get(type, PropertyIdentifierEnum::ObjectName))) {
		s.insert(ref);
	}
}

bool ObjectProperties::isSupported(ObjectTypeEnum type) {
	return getObjectPropertiesMap().find(type) != getObjectPropertiesMap().end();
}

bool ObjectProperties::isServerSupported(ObjectTypeEnum type) {
	return getObjectTypeServerSupported().isSupported(type);
}

BacnetValueRef ObjectProperties::getBacnetValue(ObjectTypeEnum type,
		PropertyIdentifierEnum id, bool throwUnsupported) {
	const auto objProps = getObjectPropertiesMap();
	auto itObj = objProps.find(type);
	if (itObj != objProps.end()) {
		ObjectPropertyRef refCmp;
		refCmp = new ObjectProperty(type, id, 0);
		auto itProp = itObj->second.find(refCmp);
		if (itProp != itObj->second.end()) {
			return (*itProp)->getDefaultProperty()->getValue()->clone();
		}
	}
	throwException(BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::UnknownProperty,
			FC::StringAPrintf("%s property of object %s does not exist", type.name(), id.name())));
	return 0;
}

PropertyRef ObjectProperties::getProperty(ObjectTypeEnum type,
		PropertyIdentifierEnum id) {
	const auto objProps = getObjectPropertiesMap();
	auto itObj = objProps.find(type);
	if (itObj != objProps.end()) {
		ObjectPropertyRef refCmp;
		refCmp = new ObjectProperty(type, id, 0);
		auto itProp = itObj->second.find(refCmp);
		if (itProp != itObj->second.end()) {
			return new Property(*((*itProp)->getDefaultProperty()));
		}
	}
	return 0;
}

const ObjectTypesSupported& ObjectProperties::getObjectTypeServerSupported() {
	static bool initialized = false;
	static ObjectTypesSupported types;

	if (!initialized) {
		#define START_OBJECT(typeEnum, supported) \
			types.setSupported(typeEnum, supported);
		#define PROPERTY(propId, required, writtable, bacValueRef)
		#define END_OBJECT
		#include "ObjectPropertiesDefinition.i"
		#undef START_OBJECT
		#undef PROPERTY
		#undef END_OBJECT
		initialized = true;
	}
	return types;
}

void ObjectProperties::getPropertiesIdSet(
		ObjectTypeEnum type, PropIdListChoice choice,
		PropertyIdentifierSet& propsSet) {

	typedef std::map<ObjectProperties::PropIdListChoice, PropertyIdentifierSet> PropIdSetMap;
	typedef std::map<ObjectTypeEnum, PropIdSetMap> ObjectPropIdMap;

	static bool initialized = false;
	static ObjectPropIdMap props;

	if (!initialized) {
		ObjectTypeEnum objType;
		#define START_OBJECT(typeEnum, supported) objType = typeEnum;
		#define PROPERTY(propId, required, writtable, bacValueRef) 				\
				if (required) {													\
					props[objType][ObjectProperties::Required].insert(propId);		\
				} else {														\
					props[objType][ObjectProperties::Optional].insert(propId);		\
				}																\
				props[objType][ObjectProperties::All].insert(propId);
		#define END_OBJECT FC_Debug1f("Created properties sets for %s", \
									ObjectTypeEnum::getName(objType));
		#include "ObjectPropertiesDefinition.i"
		#undef START_OBJECT
		#undef PROPERTY
		#undef END_OBJECT
		initialized = true;
	}
	propsSet.clear();
	if (props.find(type) != props.end()) {
		propsSet = props[type][choice];
	}
}

} // BACNET namespace


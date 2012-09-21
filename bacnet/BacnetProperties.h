/*
 * BacnetProperties.h
 *
 *  Created on: Aug 8, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetProperties_h
#define BacnetProperties_h

#include <cassert>
#include "BacnetBaseTypes.h"

namespace VIGBACNET {

/**
 * Generic BACnet Object property
 * Object property track store the value for the property.  It also
 * define if the property is required by the object and the property
 * can be written by remote devices.
 * A property is an immutable object meaning that once it has been defined
 * you cannot change the value type or the different flag
 */
class Property : public FC::RefObject, public FC::Formatter {
public:
	Property(const BacnetValueRef &value, bool isRequired = false, bool isRemoteWrittable=false) :
		_value(value), _isRemoteWrittable(isRemoteWrittable),
		_isRequired(isRequired){ }

	Property(const Property& prop) {
		copy(prop);
	}

	bool isRemoteWrittable() { return _isRemoteWrittable; }

	BacnetValueRef getValue() {
		return _value;
	}

	BacnetValueRef setValue(const BacnetValue &value) {
		if (!_value) {
			_value = value.clone();
		} else {
			_value->set(value);
		}
		return _value;
	}

	virtual void out(std::ostream &os) const {
		os << FC::StringAPrintf(
			"{Property: "
			"is required: %s, "
			"remote write: %s, "
			"value: %s}", _isRequired ? "true" : "false",
				 _isRemoteWrittable ? "true" : "false",
				 _value ? _value->toString().c_str() : "{No value}");
	}

protected:
	Property& operator=(const Property& prop);
	void copy(const Property& prop, bool checkType=false);


	FC::Ref<BacnetValue> _value;
	bool _isRemoteWrittable;
	bool _isRequired;
};
typedef FC::Ref<Property> PropertyRef;

/**
 * BACnet Object Property Identifier definition
 * This object is the definition of a property for a specific object and
 * for a specific property type.  It contains a tuple of the property id
 * associated to the object type and the default value for that Object property.
 * An ObjectProperty is an immutable object.
 */
class ObjectProperty : public FC::RefObject {
public:
	ObjectProperty(ObjectTypeEnum type, PropertyIdentifierEnum id,
				   PropertyRef prop) :
		_objType(type), _propId(id), _defaultProp(prop) {}
	ObjectProperty(const ObjectProperty& prop) {
		copy(prop);
	}

	ObjectTypeEnum getObjectType() const { return _objType; }
	PropertyIdentifierEnum getPropertyId() const { return _propId; }
	PropertyRef getDefaultProperty() const { return _defaultProp; }

protected:
	ObjectProperty& operator=(const ObjectProperty& prop);
	void copy(const ObjectProperty& prop, bool checkType=false);

	ObjectTypeEnum _objType;
	PropertyIdentifierEnum _propId;
	PropertyRef _defaultProp;
};
typedef FC::Ref<ObjectProperty> ObjectPropertyRef;

class ObjectPropertiesComparator {
public:
	typedef bool(*ObjPropCmpFnPtr)(const ObjectPropertyRef&, const ObjectPropertyRef&);

	static bool sortByObjAndPropAsc(const ObjectPropertyRef& lh, const ObjectPropertyRef& rh) {
		if (lh->getObjectType() == rh->getObjectType()) {
			return lh->getPropertyId() < rh->getPropertyId();
		}
		return lh->getObjectType() < rh->getObjectType();
	}
	static bool sortByPropAndObjAsc(const ObjectPropertyRef& lh, const ObjectPropertyRef& rh) {
		if (lh->getObjectType() == rh->getObjectType()) {
			return lh->getPropertyId() < rh->getPropertyId();
		}
		return lh->getObjectType() < rh->getObjectType();
	}
	static bool sortByObjAsc(const ObjectPropertyRef& lh, const ObjectPropertyRef& rh) {
		return lh->getObjectType() < rh->getObjectType();
	}
	static bool sortByPropAsc(const ObjectPropertyRef& lh, const ObjectPropertyRef& rh) {
		return lh->getPropertyId() < rh->getPropertyId();
	}
};

typedef std::set<ObjectPropertyRef,	ObjectPropertiesComparator::ObjPropCmpFnPtr> ObjectPropertySet;
typedef std::set<PropertyIdentifierEnum> PropertyIdentifierSet;

struct ObjectProperties {
public:
	enum PropIdListChoice {
		All = PropertyIdentifierEnum::All,
		Required = PropertyIdentifierEnum::Required,
		Optional = PropertyIdentifierEnum::Optional
	};

	static void getAll(ObjectPropertySet&);
	static void getAll(ObjectTypeEnum, ObjectPropertySet&);
	static ObjectPropertyRef get(ObjectTypeEnum, PropertyIdentifierEnum);
	static void getEssential(ObjectTypeEnum, ObjectPropertySet&);
	static void getRequired(ObjectTypeEnum, ObjectPropertySet& s);
	static BacnetValueRef getBacnetValue(ObjectTypeEnum, PropertyIdentifierEnum,
			bool throwUnsupported = true);
	static PropertyRef getProperty(ObjectTypeEnum, PropertyIdentifierEnum);
	static bool isSupported(ObjectTypeEnum);
	static bool isServerSupported(ObjectTypeEnum);
	static const ObjectTypesSupported& getObjectTypeServerSupported();
	static void getPropertiesIdSet(ObjectTypeEnum, PropIdListChoice,
			PropertyIdentifierSet&);
};


} // BACNET namespace

#endif /* BACNETPROPERTIES_H_ */

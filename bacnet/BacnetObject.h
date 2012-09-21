/*
 * BacnetObjects.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetObjects_h
#define BacnetObjects_h

#include <cassert>
#include "fc.h"
#include "BacnetUtils.h"
#include "BacnetProperties.h"
#include "BacnetValueGetterSetter.h"

namespace VIGBACNET {


class Object;
typedef FC::Ref<Object> ObjectRef;

class Object : public FC::RefObject, public FC::Formatter {
public:
	typedef std::map<PropertyIdentifierEnum, PropertyRef> PropertyMap;

	static ObjectRef create(ObjectTypeEnum type, ObjectInstance instance, std::string name="");
	static ObjectRef createLight(ObjectTypeEnum type, ObjectInstance instance, std::string name="");

	// TODO make constructor protected and only use the create method
	// to enforce only known object creation
	// Or at minimum give a validate method that make sure the object has
	// all the required properties for its type
	Object(const ObjectIdentifier& oid, std::string name = "",
			const ObjectPropertySet *props = 0) {
		init(oid, name, props);
	}

	Object(ObjectTypeEnum type, ObjectInstance instance,
			std::string name = "", const ObjectPropertySet *props = 0) {
		init(ObjectIdentifier(type, instance), name, props);
	}

	Object(const Object& object) {
		copy(object);
	}

	Object& operator=(const Object& object) {
		if (this != &object) {
			copy(object);
		}
		return *this;
	}

	ObjectIdentifier getOid() const {
		ObjectIdentifier oid;
		getProperty(PropertyIdentifierEnum::ObjectIdentifier, oid);
		return oid;
	}
	ObjectInstance instance() const { return getOid().getInstance(); }
	ObjectTypeEnum type() const { return getOid().getType(); }
	std::string name() const {
		std::string str;
		getProperty(PropertyIdentifierEnum::ObjectName, str);
		return str;
	}

	template <typename T>
	bool getProperty(PropertyIdentifierEnum id, T& value, bool throwError = true) const {
		// Make sure property exist first
		auto it = _properties.find(id);
		if (it == _properties.end()) {
			if (throwError) {
				std::ostringstream oss;
				oss << "Property " << id.name() << " of object " << name() << "does not exist.";
				throwException(BacnetErrorException(ErrorClassEnum::Property,
						ErrorCodeEnum::UnknownProperty, oss.str()));
			}
			return false;
		}
		return ValueGetter::cast(*(it->second->getValue()), value, throwError);
	}

	template <typename T>
	bool setProperty(PropertyIdentifierEnum id, const T& value, bool throwError = true) {
		// Make sure property exist first
		auto it = _properties.find(id);
		if (it == _properties.end()) {
			if (throwError) {
				std::ostringstream oss;
				oss << "Property " << id.name() << " of object " << name() <<
						"does not exist.";
				throwException(BacnetErrorException(ErrorClassEnum::Property,
						ErrorCodeEnum::UnknownProperty, oss.str()));
			}
			return false;
		}
		return ValueSetter::cast(value, *(it->second->getValue()), throwError);
	}
	bool isPropertyRemoteWrittable(PropertyIdentifierEnum id) const;
	bool isPropertyModified(PropertyIdentifierEnum id) const;
	void clearPropertyModified(PropertyIdentifierEnum id);
	bool isPropertyDirty(PropertyIdentifierEnum id) const;
	void clearPropertyDirty(PropertyIdentifierEnum id);
	/*
	 * Once created, some object properties cannot be changed.
	 * e.q. ObjectId, Name, Type
	 */
	static bool canChangeProperty(PropertyIdentifierEnum id) {
		if (id == PropertyIdentifierEnum::ObjectIdentifier ||
			id == PropertyIdentifierEnum::ObjectName ||
			id == PropertyIdentifierEnum::ObjectType) {
			return false;
		}
		return true;
	}

	virtual void out(std::ostream &os) const;

protected:
	void init(const ObjectIdentifier& oid, std::string& name, const ObjectPropertySet *props);
	//PropertyRef getProperty(PropertyIdentifierEnum prop) const;
	void copy(const Object& object);

	PropertyMap _properties;
};


} // BACNET namespace

#endif /* BACNETOBJECTS_H_ */

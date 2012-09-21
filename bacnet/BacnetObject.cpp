/*
 * BacnetObjects.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include "BacnetObject.h"

namespace VIGBACNET {

ObjectRef Object::create(ObjectTypeEnum type, ObjectInstance instance, std::string name) {
	if (instance == 0 || instance > ObjectIdentifier::MaxInstance) {
		std::string err;
		FC::StringPrintf(err, "cannot create object %s because instance (%u) is not out of range (0-%d",
				name.c_str(), instance, ObjectIdentifier::MaxInstance);
		throwException(BacnetErrorException(ErrorClassEnum::Property,
				ErrorCodeEnum::ValueOutOfRange, err));
	}
	ObjectPropertySet props(ObjectPropertiesComparator::sortByPropAsc);
	ObjectProperties::getAll(type, props);
	if (props.empty()) {
		return 0;
	}
	return new Object(type, instance, name, &props);
}

ObjectRef Object::createLight(ObjectTypeEnum type, ObjectInstance instance, std::string name) {
	if (instance == 0 || instance > ObjectIdentifier::MaxInstance) {
		std::string err;
		FC::StringPrintf(err, "cannot create object %s because instance (%u) is not out of range (0-%d",
				name.c_str(), instance, ObjectIdentifier::MaxInstance);
		throwException(BacnetErrorException(ErrorClassEnum::Property,
				ErrorCodeEnum::ValueOutOfRange, err));
	}
	ObjectPropertySet props(ObjectPropertiesComparator::sortByPropAsc);
	ObjectProperties::getEssential(type, props);
	if (props.empty()) {
		return 0;
	}
	return new Object(type, instance, name, &props);
}

bool Object::isPropertyRemoteWrittable(PropertyIdentifierEnum id) const {
	auto it = _properties.find(id);
	if (it != _properties.end()) {
		return it->second->isRemoteWrittable();
	}
	return false;
}

bool Object::isPropertyModified(PropertyIdentifierEnum id) const {
	auto it = _properties.find(id);
	if (it != _properties.end()) {
		return it->second->getValue()->isModified();
	}
	return false;
}

void Object::clearPropertyModified(PropertyIdentifierEnum id) {
	auto it = _properties.find(id);
	if (it != _properties.end()) {
		it->second->getValue()->clearModified();
	}
}

bool Object::isPropertyDirty(PropertyIdentifierEnum id) const {
	auto it = _properties.find(id);
	if (it != _properties.end()) {
		return it->second->getValue()->isDirty();
	}
	return false;
}

void Object::clearPropertyDirty(PropertyIdentifierEnum id) {
	auto it = _properties.find(id);
	if (it != _properties.end()) {
		it->second->getValue()->clearDirty();
	}
}

void Object::out(std::ostream &os) const {
	auto it = _properties.begin();
	size_t last = _properties.size();
	size_t count = 1;
	os << "{Object: ";
	while (it != _properties.end()) {
		os << "{" << PropertyIdentifierEnum::getName(it->first) << ": ";
		os << *(it->second) << "}";
		it++;
		if (count < last) {
			os << ", ";
		}
		count++;
	}
	os << "}";
}

void Object::init(const ObjectIdentifier& oid, std::string& name,
		const ObjectPropertySet *props) {
	if (name.empty()) {
		name = FC::StringAPrintf("%s-%d", ObjectType::getName(oid.getType()),
					oid.getInstance());
	}
	// If props given, transfer to map
	if (props) {
		auto it = props->begin();
		while (it != props->end()) {
			_properties[(*it)->getPropertyId()] = (*it)->getDefaultProperty();
			it++;
		}
	}
	// Override the standard minimum properties required
	_properties[PropertyIdentifierEnum::ObjectIdentifier] =
			new Property(new ObjectIdentifier(oid), true, false);
	_properties[PropertyIdentifierEnum::ObjectType] =
			new Property(new ObjectType(oid.getType()), true, false);
	_properties[PropertyIdentifierEnum::ObjectName] =
			new Property(new CharacterString(name), true, false);
}
/*
PropertyRef Object::getProperty(PropertyIdentifierEnum prop) const {
	auto it = _properties.find(prop);
	if (it == _properties.end()) {
		return 0;
	}
	return it->second;
}
*/
void Object::copy(const Object& object) {
	_properties.clear();
	auto it = object._properties.begin();
	while (it != object._properties.end()) {
		_properties[it->first] = new Property(*(it->second));
		it++;
	}
}

} // BACNET namespace




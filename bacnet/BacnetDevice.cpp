/*
 * BacnetDevice.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include "BacnetDevice.h"

namespace VIGBACNET {

void DeviceAddress::out(std::ostream& os) const {
	os << "{Device Address: ";
	os << FC::StringAPrintf("{Source Net: %d}, ", _srcNet);
	os << "{Source Mac: ";
	printMac(os, _srcMac, isLocalDevice());
	os << "}, {Router Mac: ";
	printMac(os, _routerMac, true);
	os << "}";
}

bool DeviceAddress::ipToMac(const std::string& ip, uint16_t port, MacAddress& mac) {
	union { in_addr_t in; uint16_t s; uint8_t b[4]; } au;
	if (inet_pton(AF_INET, ip.c_str(), &au)) {
		mac.assign(au.b, au.b + sizeof(au.b));
		au.s = port;
		mac.push_back(au.b[1]);
		mac.push_back(au.b[0]);
		return true;
	}
	return false;
}

bool DeviceAddress::macToIp(const MacAddress& mac, std::string& ip, uint16_t& port) {
	// Check the mac address is long enough to be of IP/Port format
	if (mac.size() < 6) {
		ip = "";
		port = 0;
		return false;
	} else {
		union { in_addr_t in; uint16_t s; uint8_t b[4]; } au;
		::memcpy(&au, &mac.at(0), 4);
		char addr[32];
		if (inet_ntop(AF_INET, &au, addr, sizeof(addr))) {
			ip = addr;
			au.b[1] = mac.at(4);
			au.b[0] = mac.at(5);
			port = au.s;
			return true;
		}
	}
	return false;
}

std::string DeviceAddress::getLocalhostIp(const std::string& interface) {
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

std::string DeviceAddress::getLocalhostBroadcast(const std::string& interface) {
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ-1);
	ioctl(fd, SIOCGIFBRDADDR, &ifr);
	close(fd);
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

void DeviceAddress::printMac(std::ostream& os, const MacAddress& mac, bool isIp) const {
	std::string msg;
	if (isIp) {
		std::string ip;
		uint16_t port;
		if (macToIp(mac, ip, port)) {
			FC::StringPrintf(msg, "%s/%Xh", ip.c_str(), port);
		}
	}
	// Maybe we failed to get a proper IP, default to byte print
	if (msg.empty()) {
		std::ostringstream oss;
		size_t last = mac.size() - 1;
		for (size_t i = 0; i < mac.size(); i++) {
			FC::CStrFormatter::hex(oss, mac[i]);
			if (i < last) {
				oss << '-';
			}
		}
		msg = oss.str();
	}
	os << msg;
}

bool Device::isPropertyRemoteWrittable(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const {
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		return it->second->isPropertyRemoteWrittable(id);
	}
	return false;
}

bool Device::isPropertyModified(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const {
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		return it->second->isPropertyModified(id);
	}
	return false;
}

void Device::clearPropertyModified(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const {
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		it->second->clearPropertyModified(id);
	}
}

bool Device::isPropertyDirty(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const {
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		return it->second->isPropertyDirty(id);
	}
	return false;
}

void Device::clearPropertyDirty(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const {
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		it->second->clearPropertyDirty(id);
	}
}

void Device::addObject(const Object& object) {
	if (object.getOid().getType() == ObjectTypeEnum::Device) {
		throwException(BacnetErrorException(ErrorClassEnum::Object,
				ErrorCodeEnum::DynamicCreationNotSupported,
				"Only one device object can exist per device."));
	}
	BacnetErrorException ex;
	if (!canAddObject(object, &ex)) {
		throwException(ex);
	}
	_objects[object.getOid()] = new Object(object);
	resetObjectInstance(object.getOid().getType());
}

void Device::deleteObject(const ObjectIdentifier& oid) {
	if (oid.getType() == ObjectTypeEnum::Device) {
		throwException(BacnetErrorException(ErrorClassEnum::Object,
				ErrorCodeEnum::ObjectDeletionNotPermitted,
				"Cannot remove a Device object from its own device."));
	}
	_objects.erase(oid);
	resetObjectInstance(oid.getType());
}

ObjectInstance Device::getNextObjectInstance(ObjectTypeEnum type) const {
	ObjectInstance instance = 1;
	auto it = _objTypeinstances.find(type);
	if (it != _objTypeinstances.end()) {
		instance = it->second + 1;
	}
	return instance;
}

ObjectRef Device::getObject(const ObjectIdentifier& oid) {
	ObjectRef obj;
	auto it = _objects.find(oid);
	if (it != _objects.end()) {
		obj = new Object(*(it->second));
	}
	return obj;
}

ObjectRef Device::getObject(const std::string &name) {
	ObjectRef obj;
	auto it = _objects.begin();
	while (it != _objects.end()) {
		if (it->second->name() == name) {
			obj = new Object(*(it->second));
		}
		it++;
	}
	return obj;
}
/**
 * Iterate through the device object to return the next object following OID
 * If {from} is null, the first object is selected.  If no more object a null
 * reference is returned.
 * The method return a reference to a newly created copy of the object selected.  It
 * prevents having external application mess with device object state.
 *
 * arguments:
 * [in] from pointer to the starting object identifier
 *
 * return reference to copy of next object found or null if none
 */
ObjectRef Device::getNextObject(const ObjectIdentifier* from) {
	ObjectRef obj;
	if (from) {
		auto it = _objects.upper_bound(*from);
		if (it != _objects.end()) {
			obj = new Object(*(it->second));
		}
	} else if (_objects.size()) {
		obj = new Object(*(_objects.begin()->second));
	}
	return obj;
}
/**
 * Print address and all objects for this device
 */
void Device::out(std::ostream& os) const {
	auto it = _objects.begin();
	size_t last = _objects.size();
	size_t count = 1;
	os << "{Device: ";
	os << _address;
	os << "} {Objects: ";
	while (it != _objects.end()) {
		os << "{" << it->first << ": ";
		os << *(it->second) << "}";
		it++;
		if (count < last) {
			os << ", ";
		}
		count++;
	}
	os << "}}";
}
/**
 * Look for highest instance of a specific object type and save it
 */
void Device::resetObjectInstance(const ObjectTypeEnum& type) {
	ObjectInstance instance = 0;
	auto itObj = _objects.begin();
	while (itObj != _objects.end()) {
		if (itObj->first.getType() == type) {
			instance = std::max(instance, itObj->first.getInstance());
		}
		itObj++;
	}
	_objTypeinstances[type] = instance;
}

/*
 * Check that an similar oid does not exist and the name
 * is unique among all oid
 */
bool Device::canAddObject(const Object& obj, BacnetErrorException* ex) const {
	auto it = _objects.begin();
	while (it != _objects.end()) {
		if (it->second->getOid() == obj.getOid()) {
			if (ex) {
				*ex = BacnetErrorException(ErrorClassEnum::Object,
					ErrorCodeEnum::ObjectIdentifierAlreadyExists,
					FC::StringAPrintf("Object %s already exist.",
							obj.getOid().getType().name()));
			}
			return false;
		} else if (it->second->name() == obj.name()) {
			if (ex) {
				*ex = BacnetErrorException(ErrorClassEnum::Object,
					ErrorCodeEnum::ObjectIdentifierAlreadyExists,
					FC::StringAPrintf("Object %s already exist.", obj.name().c_str()));
			}
			return false;
		}
		it++;
	}
	return true;
}

void Device::copy(const Device& dev) {
	_address = dev._address;
	_objTypeinstances = dev._objTypeinstances;
	auto it = dev._objects.begin();
	while (it != dev._objects.end()) {
		ObjectRef ref = new Object(*(it->second));
		_objects.insert(ObjectMap::value_type(it->first, ref));
		if (it->first == ObjectTypeEnum::Device) {
			_device = ref;
		}
		it++;
	}
}

} // VIGBACNET namespace


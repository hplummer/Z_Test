/*
 * BacnetDevice.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetDevice_h
#define BacnetDevice_h

#include <arpa/inet.h>

#include "BacnetValue.h"
#include "BacnetObject.h"

namespace VIGBACNET {

/**
 * Store information on how to access a BACnet device
 * This class stores the information needed to access a
 * BACnet device.  A device can be on the local network, indicated
 * by a source network of 0 or it can be on remote BACnet network
 * and access is through a router.
 * Because we only know about IP for local device or router, some
 * helper function are provided to set and get ip address.
 * If the device is remote the source address is of BACnet format, aka
 * byte array with most significant byte first.
 */
class DeviceAddress : public FC::Formatter {
public:
	static const uint16_t BacnetPort = 0xBAC0;
	typedef std::vector<uint8_t> MacAddress;

	DeviceAddress() : _srcNet(0) {
		ipToMac(getLocalhostIp(), BacnetPort, _srcMac);
	}

	DeviceAddress(const MacAddress& srcMac, uint16_t srcNet = 0,
				  const MacAddress& routerMac = MacAddress(0)) :
		_srcMac(srcMac), _srcNet(srcNet), _routerMac(routerMac) {
	}

	DeviceAddress(const uint8_t* srcMac, uint8_t srcLen, uint16_t srcNet = 0,
				  const uint8_t* routerMac = 0, uint8_t routerLen = 0) :
					  _srcNet(srcNet) {
		if (srcMac) {
			_srcMac.assign(srcMac, srcMac + srcLen);
		}
		if (routerMac) {
			_routerMac.assign(routerMac, routerMac + routerLen);
		}
	}

	DeviceAddress(const std::string& srcIp, uint16_t srcPort, uint16_t srcNet = 0,
				  const std::string& routerIp = "", uint16_t routerPort = 0xBAC0) :
					  _srcNet(srcNet) {
		if (!srcIp.empty()) {
			ipToMac(srcIp, srcPort, _srcMac);
		}
		if (!routerIp.empty()) {
			ipToMac(routerIp, routerPort, _routerMac);
		}
	}

	DeviceAddress(const DeviceAddress& addr) {
		copy(addr);
	}

	DeviceAddress& operator=(const DeviceAddress& addr) {
		if (this != &addr) {
			copy(addr);
		}
		return *this;
	}

	MacAddress getSourceMac() const {
		return _srcMac;
	}

	bool getSourceIp (std::string& ip, uint16_t& port) const {
		return macToIp(_srcMac, ip, port);
	}

	void setSourceMac(const MacAddress& mac) {
		_srcMac = mac;
	}

	void setSourceMac(const uint8_t* mac, uint8_t len) {
		_srcMac.clear();
		if (mac) {
			_srcMac.insert(_srcMac.begin(), mac, mac + len);
		}
	}

	void setSourceIp(const std::string& ip, uint16_t port) {
		ipToMac(ip, port, _srcMac);
	}

	uint16_t getSourceNet() const {
		return _srcNet;
	}

	void setSourceNet(uint16_t net) {
		_srcNet = net;
	}

	MacAddress getRouteMac() const {
		return _routerMac;
	}

	bool getRouterIp (std::string& ip, uint16_t& port) const {
		return macToIp(_routerMac, ip, port);
	}

	void setRouterMac(const MacAddress& mac) {
		_routerMac = mac;
	}

	void setRouterMac(const uint8_t* mac, uint8_t len) {
		_routerMac.clear();
		if (mac) {
			_routerMac.insert(_routerMac.begin(), mac, mac + len);
		}
	}

	void setRouterIp(const std::string& ip, uint16_t port) {
		ipToMac(ip, port, _routerMac);
	}

	bool hasRouter() const {
		return !(_routerMac.empty());
	}

	bool isLocalDevice() const {
		return (_srcNet == 0);
	}

	/**
	 * Print address and all objects for this device
	 */
	virtual void out(std::ostream& os) const;
	static bool ipToMac(const std::string& ip, uint16_t port, MacAddress& mac);
	static bool macToIp(const MacAddress& mac, std::string& ip, uint16_t& port);
	static std::string getLocalhostIp(const std::string& interface = "eth0");
	static std::string getLocalhostBroadcast(const std::string& interface = "eth0");

private:


	void copy(const DeviceAddress& addr) {
		_srcNet = addr._srcNet;
		_srcMac = addr._srcMac;
		_routerMac = addr._routerMac;
	}

	void printMac(std::ostream& os, const MacAddress& mac, bool isIp=false) const;

	// Maybe split into Network address for BACnet remote network
	// and DataLinkAddress for IP address
	MacAddress _srcMac;
	uint16_t _srcNet;
	MacAddress _routerMac;
};


class Device;
typedef FC::Ref<Device> DeviceRef;

class Device : public FC::RefObject, public FC::Formatter {
public:
	Device(ObjectInstance instance, const std::string &name = "") {
		_device = Object::create(ObjectTypeEnum::Device, instance, name);
		// Add the device to the object list
		_objects[_device->getOid()] = _device;
	}

	Device(const Device& device) {
		copy(device);
	}

	Device& operator=(const Device& device) {
		if (this != &device) {
			copy(device);
		}
		return *this;
	}

	void setAddress(const DeviceAddress& addr) {
		_address = addr;
	}

	void setAddress(const std::string& ip, uint16_t port) {
		_address.setSourceIp(ip, port);
	}

	void setRouterAddress(const std::string& ip, uint16_t port) {
		_address.setRouterIp(ip, port);
	}

	void setNetwork(uint16_t net) {
		_address.setSourceNet(net);
	}

	DeviceAddress getAddress() const {
		return _address;
	}

	bool getAddress(std::string& ip, uint16_t& port) const {
		return _address.getSourceIp(ip, port);
	}

	bool getRouterAddress(std::string& ip, uint16_t& port) {
		return _address.getRouterIp(ip, port);
	}

	uint16_t getNetwork() const {
		return _address.getSourceNet();
	}

	ObjectInstance getInstance() const {
		return _device->getOid().getInstance();
	}

	std::string getName() const {
		return _device->name();
	}

	template <typename T>
	bool getProperty(PropertyIdentifierEnum id, T& value, bool throwError = true) const {
		return _device->getProperty(id, value, throwError);
	}
	template <typename T>
	bool getObjectProperty(ObjectTypeEnum type, ObjectInstance instance, PropertyIdentifierEnum id,
						   T &value, bool throwError = true) const {
		return getObjectProperty(ObjectIdentifier(type, instance), id, value, throwError);
	}
	template <typename T>
	bool getObjectProperty(const ObjectIdentifier &oid, PropertyIdentifierEnum id,
						   T &value, bool throwError = true) const {
		auto it = _objects.find(oid);
		if (it != _objects.end()) {
			it->second->getProperty(id, value, throwError);
			return true;
		} else if (throwError) {
			std::ostringstream oss;
			oss << "Object " << oid << "does not exist.";
			throwException(BacnetErrorException(ErrorClassEnum::Object,
					ErrorCodeEnum::UnknownObject, oss.str()));
		}
		return false;
	}

	template <typename T>
	bool setProperty(PropertyIdentifierEnum id, const T& value, bool throwError = true) {
		return _device->setProperty(id, value, throwError);
	}
	template <typename T>
	bool setObjectProperty(ObjectTypeEnum type, ObjectInstance instance, PropertyIdentifierEnum id,
						   const T &value, bool throwError = true) {
		return setObjectProperty(ObjectIdentifier(type, instance), id, value, throwError);
	}
	template <typename T>
	bool setObjectProperty(const ObjectIdentifier &oid, PropertyIdentifierEnum id,
						   const T &value, bool throwError = true) {
		auto it = _objects.find(oid);
		if (it != _objects.end()) {
			it->second->setProperty(id, value, throwError);
			return true;
		} else if (throwError) {
			std::ostringstream oss;
			oss << "Object " << oid << "does not exist.";
			throwException(BacnetErrorException(ErrorClassEnum::Object,
					ErrorCodeEnum::UnknownObject, oss.str()));
		}
		return false;
	}
	bool isPropertyRemoteWrittable(PropertyIdentifierEnum id) const {
		return _device->isPropertyRemoteWrittable(id);
	}
	bool isPropertyRemoteWrittable(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const;
	bool isPropertyModified(PropertyIdentifierEnum id) const {
		return _device->isPropertyModified(id);
	}
	bool isPropertyModified(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const;
	void clearPropertyModified(PropertyIdentifierEnum id) const {
		_device->clearPropertyModified(id);
	}
	void clearPropertyModified(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const;
	bool isPropertyDirty(PropertyIdentifierEnum id) const {
		return _device->isPropertyDirty(id);
	}
	bool isPropertyDirty(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const;
	void clearPropertyDirty(PropertyIdentifierEnum id) const {
		_device->clearPropertyDirty(id);
	}
	void clearPropertyDirty(const ObjectIdentifier &oid, PropertyIdentifierEnum id) const;

	void addObject(const Object& object);
	void deleteObject(const ObjectIdentifier& oid);
	ObjectInstance getNextObjectInstance(ObjectTypeEnum type) const;
	ObjectRef getObject(const ObjectIdentifier& oid);
	ObjectRef getObject(const std::string &name);
	bool hasObject(const ObjectIdentifier& oid) {
		return _objects.find(oid) != _objects.end();
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
	ObjectRef getNextObject(const ObjectIdentifier* from = 0);
	size_t getCount() {
		return _objects.size();
	}
	/**
	 * Print address and all objects for this device
	 */
	virtual void out(std::ostream& os) const;

protected:
	/**
	 * Look for highest instance of a specific object type and save it
	 */
	void resetObjectInstance(const ObjectTypeEnum& type);
	/*
	 * Check that an similar oid does not exist and the name
	 * is unique among all oid
	 */
	bool canAddObject(const Object& obj, BacnetErrorException* ex = 0) const;
	void copy(const Device& dev);

	struct SortByOid {
		bool operator() (const ObjectIdentifier& lhs,
						 const ObjectIdentifier& rhs) const {
			if (lhs.getType() == rhs.getType()) {
				return lhs.getInstance() < rhs.getInstance();
			}
			return lhs.getType() < rhs.getType();
		}
	};

	typedef std::map<ObjectIdentifier, FC::Ref<Object>, SortByOid> ObjectMap;
	typedef std::map<ObjectTypeEnum, ObjectInstance> ObjectInstanceMap;

	DeviceAddress _address;
	ObjectMap _objects;
	ObjectInstanceMap _objTypeinstances;
	ObjectRef _device;
};

} // VIGBACNET



#endif /* BACNETDEVICE_H_ */

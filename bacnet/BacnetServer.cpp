/*
 * BacnetServer.cpp
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */


#include <sys/time.h>
#include "fc.h"
#include "FCStopWatch.h"
#include "BacnetServer.h"
#include "vsbhp.h"
#include "mstp.h"
#include "BacnetVsbConverter.h"


namespace VIGBACNET {

PropertiesSetter defaultPropertiesSetter = {
	123456,													// instance
	"Vigilent Server",										// name
	DeviceStatus::Operational,								// status
	"Vigilent",												// vendor name
	282,													// vendor id
	"Vigilent EMS",											// model name
	"2.0",													// firmware version
	"3.4.0",												// software version
	"Server Room",											// location
	"Vigilent EMS BACnet Server",							// description
	REQUEST_TIMEOUT,										// APDU timeout
	REQUEST_RETRIES,										// APDU retries
	1,														// Database version
	"",														// BBMD IP
	2,														// BBMD TTL
	DeviceAddress(DeviceAddress::getLocalhostIp(), 0xBAC0),	// Device Address
	DeviceAddress::getLocalhostBroadcast(),					// Device Broadcast Address,
	5 														// stack process rate in msec
};

ServerManager::InstanceServerMap ServerManager::_servers;

ServerRef ServerManager::createServer(const PropertiesSetter& props) {
	if (_servers.size() < MaxServerAllowed &&
		_servers.find(props.deviceInstance) == _servers.end()) {
		ServerRef server = new Server(props.deviceInstance, props.deviceName, props.processRate);
		setServerProperties(server, props);
		_servers[props.deviceInstance] = server;
		return server;
	}
	return 0;
}

bool ServerManager::deleteServer(ObjectInstance serverInstance) {
	return _servers.erase(serverInstance) > 0;
}

ServerRef ServerManager::getServer(ObjectInstance serverInstance) {
	auto it = _servers.find(serverInstance);
	return (it != _servers.end()) ? it->second : ServerRef(0);
}

void ServerManager::setServerProperties(const ServerRef& server, const PropertiesSetter& props) {
	server->setProperty(PropertyIdentifierEnum::SystemStatus,
			props.systemStatus);
	server->setProperty(PropertyIdentifierEnum::VendorName,
			props.vendorName);
	server->setProperty(PropertyIdentifierEnum::VendorIdentifier,
			props.vendorId);
	server->setProperty(PropertyIdentifierEnum::ModelName,
			props.modelName);
	server->setProperty(PropertyIdentifierEnum::FirmwareRevision,
			props.firmwareRevision);
	server->setProperty(PropertyIdentifierEnum::ApplicationSoftwareVersion,
			props.softwareVersion);
	server->setProperty(PropertyIdentifierEnum::Location,
			props.location);
	server->setProperty(PropertyIdentifierEnum::Description,
			props.description);
	server->setProperty(PropertyIdentifierEnum::ApduTimeout,
			props.apduTimeout);
	server->setProperty(PropertyIdentifierEnum::NumberOfApduRetries,
			props.apduRetries);
	server->setProperty(PropertyIdentifierEnum::DatabaseRevision,
			props.databaseRevision);
	server->setProperty(PropertyIdentifierEnum::ProtocolObjectTypesSupported,
			ObjectProperties::getObjectTypeServerSupported());
	server->setBbmdIp(props.bbmdIp);
	server->setBbmdTtl(props.bbmdTtl);
	server->setAddress(props.address);
	server->setBroadcast(props.broadcastAddress);
}


TransactionRef TransactionManager::createTransaction(const ConfirmedServiceChoiceEnum& service,
		ConfirmedRequestAckRef ack) {
	TransactionRef ref = new Transaction(++_currentId, service, ack);
	_uuidTransMap[ref->transId()] = ref;
	_vbagTransMap[ref->vbag()] = ref;
	FC_Debug1f("Created Bacnet transaction %llu", ref->transId());
	return ref;
}


void TransactionManager::deleteTransaction(TransactionRef trans) {
	if (trans) {
		FC_Debug1f("Delete Bacnet transaction %llu", trans->transId());
		_uuidTransMap.erase(trans->transId());
		_vbagTransMap.erase(trans->vbag());
	}
}

void TransactionManager::deleteTransaction(const Transaction::IdType& transId) {
	TransactionRef ref;
	auto it = _uuidTransMap.find(transId);
	if (it != _uuidTransMap.end()) {
		deleteTransaction(it->second);
	}
}

void TransactionManager::deleteTransaction(const frVbag& bag) {
	TransactionRef ref;
	auto it = _vbagTransMap.find((frVbag*)&bag);
	if (it != _vbagTransMap.end()) {
		deleteTransaction(it->second);
	}
}

TransactionRef TransactionManager::getTransaction(const Transaction::IdType& transId) {
	TransactionRef trans;
	auto it = _uuidTransMap.find(transId);
	if (it != _uuidTransMap.end()) {
		trans = it->second;
	}
	return trans;
}

TransactionRef TransactionManager::getTransaction(const frVbag& bag) {
	TransactionRef trans;
	auto it = _vbagTransMap.find((frVbag*)&bag);
	if (it != _vbagTransMap.end()) {
		trans = it->second;
	}
	return trans;
}

ConfirmedRequestAckRef TransactionManager::getAck(const Transaction::IdType& transId) {
	ConfirmedRequestAckRef ack;
	auto it = _uuidTransMap.find(transId);
	if (it != _uuidTransMap.end()) {
		ack = it->second->ack();
	}
	return ack;
}

Transaction::State TransactionManager::getState(const Transaction::IdType& id) const {
	auto it = _uuidTransMap.find(id);
	if (it != _uuidTransMap.end()) {
		return it->second->state();
	}
	return Transaction::Dead;
}

void TransactionManager::extendTransactionLife(const Transaction::IdType& id) {
	auto it = _uuidTransMap.find(id);
	if (it != _uuidTransMap.end()) {
		it->second->resetCreateTime();
		it->second->resetCompleteTime();
	}
}
/**
 * Perform some cleanup on all transactions
 * Transaction that are just detected to be completed but never marked as complete
 * will have their timer started.  Expired transaction will be deleted.
 */
void TransactionManager::cleanup() {
	time_t now = time(0);
	auto it = _uuidTransMap.begin();
	while (it != _uuidTransMap.end()) {
		TransactionRef trans = it->second;
		if (trans) {
			// Start complete transaction timer for newly discovered complete transaction
			if (trans->vbag()->status == vbsComplete && (now - trans->completeTime()) == 0) {
				trans->resetCompleteTime();
			}
			if (((now - trans->createTime()) > RecycleTime) ||
				 (trans->vbag()->status == vbsComplete &&
				 (now - trans->completeTime()) > LiveTime)) {
				deleteTransaction(it++->first);
			} else {
				++it;
			}
		} else {
			throwException(BacnetApplicationException(FC::StringAPrintf(
					"Found Null transaction for %llu, application resources might be compromised.",
					it->first)));
			++it;
		}
	}
}

Server::Server(ObjectInstance instance, const std::string& name, unsigned doWorkRateMsec) :
	_bbmdIp("0.0.0.0"), _bbmdTtl(0), _broadcast(""), _started(false) , _workRate(doWorkRateMsec),
	_transMgr(new TransactionManager()) {
	_localDev = new Device(instance, name);
}

void Server::doWork() {
	FC::MutexLock lock(_mutex);
	struct timeval now;
	gettimeofday(&now, NULL);
	unsigned elapsedms = unsigned((now.tv_sec - _lastWork.tv_sec) * 1000 +
			(now.tv_usec - _lastWork.tv_usec) / 1000);
	// Prevent frWork to be called more than DoWorkRate or it might messup the EverySecond count
	bool elapsedOk = (elapsedms >= DoWorkRate);
	if (_started) {
		frMain();
		if (elapsedOk) {
			frWork((byte)elapsedms);
			gettimeofday(&_lastWork, NULL);
		}
		_transMgr->cleanup();
	}
}

void Server::initialize() {
	FC_Debug1("Starting the BACnet server");
	frStartup(portBIP);
	_started = true;
	gettimeofday(&_lastWork, NULL);
	on(&Server::onDoWork);
	_workTimer = new DoWorkTimer(_workRate);
	_workTimer->start(this);
}

void Server::onDoWork(DoWorkTimer *event) {
	doWork();
}
void Server::fini() {
	FC_Debug1("Stopping the BACnet server");
	_workTimer->stop();
	_started = false;
	frStop(portBIP);
	EventThread::fini();
}

void Server::sendWhoIs(const WhoIsRequest& request) const {
	FC::MutexLock lock(_mutex);
	frcForceRangeWhoIs(request.min(), request.max());
}

void Server::sendIAm() const {

}

/**
 * Send a read property request to a remote device
 *
 * Before sending the request a number of check are performed to make sure we sopport the
 * object property we are trying to read and that the server is availabel to send the request.
 */
Transaction::IdType Server::sendReadProperty(ObjectInstance device,
		const ReadPropertyRequest& request) const {
	FC::MutexLock lock(_mutex);
	int result = 0;
	BacnetValueRef value = ObjectProperties::getBacnetValue(request.oid().getType(), request.pid());
	ConfirmedRequestAckRef ack = new ReadPropertyAck(request.oid(), request.pid(), *value, request.index());
	TransactionRef trans = _transMgr->createTransaction(request.service(), ack);
	trans->vbag()->narray = (byte)request.index();
	result = frcReadProperty(device, request.oid().getCoded(), request.pid().get(),
			request.index().get(), trans->vbag());
	if (result != 0) {
		_transMgr->deleteTransaction(trans);
		Error err = VsbConverter::fromError((uint16_t)result);
		throwException(BacnetErrorException(err.getClass(), err.getCode(), FC::StringAPrintf(
				"Could not read %s-%d of device %d", request.oid().getType().name(),
				request.oid().getInstance(), device)));
	} else {
		std::ostringstream oss;
		oss << "Successfully sent read transaction (" << trans->transId() << "): " <<
				    "request: " << request << ", ack: " << *ack;
		FC_Debug1(oss.str().c_str());
	}
	return trans->transId();
}

Transaction::IdType Server::sendWriteProperty(ObjectInstance device,
		const WritePropertyRequest& request) const {
	FC::MutexLock lock(_mutex);
	int result = 0;
	ConfirmedRequestAckRef ack = new WritePropertyAck(request.oid(), request.pid());
	TransactionRef trans = _transMgr->createTransaction(request.service(), ack);
	if (VsbConverter::toVbag(*request.value(), *(trans->vbag()))) {;
		trans->vbag()->priority = (byte)request.priority();
		trans->vbag()->narray = (byte)request.index();
		result = frcWriteProperty(device, request.oid().getCoded(), request.pid().get(),
					request.index().get(), trans->vbag());
		if (result != 0) {
			_transMgr->deleteTransaction(trans);
			Error err = VsbConverter::fromError((uint16_t)result);
			throwException(BacnetErrorException(err.getClass(), err.getCode(), FC::StringAPrintf(
					"Could not write %s-%d of device %d", request.oid().getType().name(),
					request.oid().getInstance(), device)));
		} else {
			std::ostringstream oss;
			oss << "Successfully sent write transaction (" << trans->transId() << "): " <<
				    "request: " << request << ", ack: " << *ack;
			FC_Debug1(oss.str().c_str());
		}
	} else {
		throwException(BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::DatatypeNotSupported,
				FC::StringAPrintf("Datatype %s is not supported at the moment.",
						request.value()->typeName())));
	}
	return trans->transId();
}

template<>
void Server::handleUnconfirmedRequest(const DeviceAddress& source, const IAmRequest& request) {
	// Ignore ourself and all known devices
	unsigned int devInstance = request.oid().getInstance();
	if (devInstance != getInstance()  &&
		!knowsRemoteDevice(devInstance)) {
		// get the address of the device, check if it has a router in between
		DeviceRef device = new Device(devInstance);
		device->setAddress(source);
		device->setProperty(PropertyIdentifierEnum::MaxApduLengthAccepted, request.maxApduLength());
		device->setProperty(PropertyIdentifierEnum::VendorIdentifier, request.vendorId());
		device->setProperty(PropertyIdentifierEnum::SegmentationSupported, request.segmentationSupported());
		addRemoteDevice(*device);
		std::ostringstream oss;
		oss << "I Am Request -- Added new device " << *device;
		FC_Debug(oss.str());
		FC::Ref<IAmEvent> event(new IAmEvent(request));
		post(event);
	}
}


template<>
void Server::handleConfirmedRequest(const ReadPropertyRequest &request, ReadPropertyAckRef &ack) {
	try {
		std::ostringstream oss;
		oss << "Got a read request: " << request;
		FC_Debug1(oss.str().c_str());
		try {
			// can throw an BacnetErrorException if property does not match object
			BacnetValueRef val = ObjectProperties::getBacnetValue(request.oid().getType(), request.pid());
			_localDev->getObjectProperty(request.oid(), request.pid(), *val);
			ack = new ReadPropertyAck(request.oid(), request.pid(), *val, request.index());
			notifyRequest<ReadRequestEvent>(request);
		} catch(BacnetApplicationException& ex) {
			// Refactor any application exception to be a valid BACnet error
			throw BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType);
		}
	} catch (std::bad_cast&) {
		throw BacnetErrorException(ErrorClassEnum::Services, ErrorCodeEnum::MissingRequiredParameter);
	}
}

template<>
void Server::handleConfirmedRequest(const WritePropertyRequest &request, WritePropertyAckRef &ack) {
	try {
		std::ostringstream oss;
		oss << "Got a write request: " << request;
		FC_Debug1(oss.str().c_str());
		// Check if the server has this object
		if (_localDev->hasObject(request.oid())) {
			// check if the property is remote writtable
			if (_localDev->isPropertyRemoteWrittable(request.oid(), request.pid())) {
				_localDev->setObjectProperty(request.oid(), request.pid(), request.value());
				notifyRequest<WriteRequestEvent>(request);
			} else {
				throw BacnetErrorException(ErrorClassEnum::Property, ErrorCodeEnum::WriteAccessDenied);
			}
		} else {
			throw BacnetErrorException(ErrorClassEnum::Object, ErrorCodeEnum::UnknownObject);
		}

	} catch (std::bad_cast&) {
		throw BacnetErrorException(ErrorClassEnum::Services, ErrorCodeEnum::MissingRequiredParameter);
	}
}

void Server::handleConfirmedRequestAck(const Transaction& trans) {
	switch (trans.service()) {
	case ConfirmedServiceChoiceEnum::ReadProperty:
		handleReadAck(trans);
		break;
	case ConfirmedServiceChoiceEnum::WriteProperty:
		handleWriteAck(trans);
		break;
	default:
		break;
	}
}

void Server::handleReadAck(const Transaction& trans) {
	ReadPropertyAckRef ack = dynamic_cast<ReadPropertyAck*>(trans.ack().get());
	std::ostringstream oss;
	oss << "Got a read transaction ack (" << trans.transId() << ") with ack: " << *ack;
	FC_Debug1(oss.str().c_str());
	BacnetValueRef value = VsbConverter::fromVbag(*(trans.vbag()));
	FC::Ref<Error> error;
	if (trans.hasError()) {
		error = value_cast<Error*>(value.get(), false);
		if (!error) {
			error = new Error(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType);
		}
	} else if (!value) {
		error = new Error(ErrorClassEnum::Property, ErrorCodeEnum::DatatypeNotSupported);
	} else if (!ack) {
		error = new Error(ErrorClassEnum::Services, ErrorCodeEnum::MissingRequiredParameter);
	} else {
		try {
			ack->value()->set(*value);
		} catch (BacnetErrorException &ex) {
			std::ostringstream oss;
			oss << "Transaction " << trans.transId() << " for " << ack->oid() <<
				"could not read the return value: " << ex.what();
			FC_Debug1(oss.str().c_str());
			error = new Error(ex.eClass(), ex.eCode());
		}
	}
	if (error) {
		notifyAck<ErrorEvent>(trans.transId(), *error);
	} else {
		notifyAck<ReadAckEvent>(trans.transId(), *ack);
	}
}

void Server::handleWriteAck(const Transaction& trans) {
	WritePropertyAckRef ack = dynamic_cast<WritePropertyAck*>(trans.ack().get());
	std::ostringstream oss;
	oss << "Got a write transaction ack (" << trans.transId() << ") with ack: " << *ack;
	FC_Debug1(oss.str().c_str());
	if (trans.hasError()) {
		BacnetValueRef value = VsbConverter::fromVbag(*(trans.vbag()));
		FC::Ref<Error> error = value_cast<Error*>(value.get(), false);
		if (!error) {
			error = new Error(ErrorClassEnum::Property, ErrorCodeEnum::InvalidDataType);
		}
		notifyAck<ErrorEvent>(trans.transId(), *error);
	} else {
		notifyAck<WriteAckEvent>(trans.transId(), *ack);
	}
}

Transaction::State Server::getTransactionState(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	TransactionRef trans = _transMgr->getTransaction(id);
	if (trans) {
		return trans->state();
	}
	return Transaction::Dead;
}

BacnetValueRef Server::getTransactionValue(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	TransactionRef trans = _transMgr->getTransaction(id);
	if (trans->state() == Transaction::Complete) {
		return VsbConverter::fromVbag(*(trans->vbag()));
	}
	return 0;
}

bool Server::isTransactionSimpelAck(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	TransactionRef trans = _transMgr->getTransaction(id);
	return trans->isSimpleAck();

}

bool Server::isTransactionError(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	TransactionRef trans = _transMgr->getTransaction(id);
	return trans->hasError();
}

void Server::deleteTransaction(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	_transMgr->deleteTransaction(id);
}


TransactionRef Server::getTransactionHandle(const Transaction::IdType& id) const {
	FC::MutexLock lock(_mutex);
	return _transMgr->getTransaction(id);
}

TransactionRef Server::getTransactionHandle(const frVbag& id) const {
	FC::MutexLock lock(_mutex);
	return _transMgr->getTransaction(id);
}

std::string Server::toString(std::string *str) {
	std::ostringstream oss;
	auto it = _remoteDev.begin();
	size_t last = _remoteDev.size();
	size_t count = 1;
	oss << "{Server: {Local: ";
	oss << *_localDev;
	oss << "}, {Remote: ";
	while (it != _remoteDev.end()) {
		oss << "{" << *(it->second) << "}";
		it++;
		if (count < last) {
			oss << ", ";
		}
		count++;
	}
	oss << "}}";
	if (str) {
		*str = oss.str();
	}
	return oss.str();
}

struct StackAccessor {
	static const DeviceRef getDevice(const Server& server) {
		return server._localDev;
	}
	template <typename REQUEST, typename ACK>
	static const void handleConfirmedRequest(Server& server, const REQUEST& request,
			ACK& ack) {
		server.handleConfirmedRequest(request, ack);
	}
	static const void handleConfirmedAck(Server& server, const Transaction& trans) {
		server.handleConfirmedRequestAck(trans);
	}
	template <typename REQUEST>
	static const void handleUnconfirmedRequest(Server& server, const DeviceAddress& source,
			const REQUEST& request) {
		server.handleUnconfirmedRequest(source, request);
	}
	static const TransactionRef getTransactionHandle(Server& server, const frVbag& bag) {
		return server.getTransactionHandle(bag);
	}
};

} //VIGBACNET

using namespace VIGBACNET;

int   bpublic fraAcknowledgeAlarm(frAckAlarm *) {
	return rejectreason + RejectReasonEnum::UnrecognizedService;
}

int   bpublic fraAtomicReadFile(dword, dword, word *, byte *, bool *) {
	return rejectreason + RejectReasonEnum::UnrecognizedService;
}

int   bpublic fraAtomicWriteFile(dword, dword *, word, byte *) {
	return rejectreason + RejectReasonEnum::UnrecognizedService;
}

bool  bpublic fraCanDoAlarms(void) {
	return false;
}

bool  bpublic fraCanDoCOV(void) {
	return false;
}

bool  bpublic fraCanDoFiles(void) {
	return false;
}

bool  bpublic fraCanDoPvtTransfer(void) {
	return false;
}

bool  bpublic fraCanDoRWPM(void) {
	return false;
}

bool  bpublic fraCanDoTimeSync(void) {
	return false;
}

void  bpublic fraGetAlarms(byte,word *, void *,bool *) {

}

dword bpublic fraGetDatabaseRevision(void) {
	return 1.0;
}

void bpublic fraIAm(frBACnetdevice* device) {
	ServerRef server = ServerManager::begin()->second;
	// get the address of the device, check if it has a router in between
	DeviceAddress addr;
	addr.setSourceNet(device->src.snet.w);
	// Device is behind a router
	if (device->src.drlen) {
		addr.setSourceMac(device->src.dradr, device->src.drlen);
		addr.setRouterMac(device->src.sadr, device->src.slen);
	} else {
		addr.setSourceMac(device->src.sadr, device->src.slen);
	}
	IAmRequest request(ObjectIdentifier(ObjectTypeEnum::Device, device->devinst),
			device->maxlen, SegmentationEnum::NoSegmentation, device->vendorid);

	StackAccessor::handleUnconfirmedRequest(*server, addr, request);
}


void  bpublic fraGetDeviceInfo(frDevInfo *dev) {
	ServerRef server = ServerManager::begin()->second;
	ObjectIdentifier oid;
	server->getProperty(PropertyIdentifierEnum::ObjectIdentifier, oid);
	dev->deviceinstance = oid.getInstance();
	Unsigned ui;
	server->getProperty(PropertyIdentifierEnum::VendorIdentifier, ui);
	dev->vendor_id = (word)ui;
	dev->maxmaster = 0;
	dev->mstpstation = 0;
	dev->maxinfoframes = 0;
	std::string str;
	uint16_t w;
	server->getAddress().getSourceIp(str, w);
	struct in_addr addr;
	inet_aton(str.c_str(),&addr);
	dev->bipipaddr = addr.s_addr;
	str = server->getBroadcast();
	inet_aton(str.c_str(),&addr);
	dev->bipipsubnet = addr.s_addr;
	dev->bipudp = w;
	str = server->getBbmdIp();
	inet_aton(str.c_str(),&addr);
	dev->bbmdipaddr = addr.s_addr;
	dev->bbmdttl = server->getBbmdTtl();
	DeviceStatus status;
	server->getProperty(PropertyIdentifierEnum::SystemStatus, status);
	dev->system_status = (byte)status;
	ObjectTypesSupported objSup;
	server->getProperty(PropertyIdentifierEnum::ProtocolObjectTypesSupported, objSup);
	VsbConverter::toObjectTypeSupported(objSup, dev->object_types_supported, 3);

	server->getProperty(PropertyIdentifierEnum::ApduTimeout, ui);
	dev->apdu_timeout = (byte)ui;
	dev->utcoffset = (short)Time::getUtcOffset();
	server->getProperty(PropertyIdentifierEnum::NumberOfApduRetries, ui);
	dev->apduretries = (byte)ui;

	/**
	 * All the converted character string are rturning a pointer
	 * to a newly created frString.  Because internally VSB
	 * uses those string until the all applicaiton exit, it is fine
	 * not to free the string.
	 */
	CharacterString charStr;
	server->getProperty(PropertyIdentifierEnum::ObjectName, charStr);
	if (dev->device_name) {
		delete dev->device_name;
	}
	dev->device_name = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::VendorName, charStr);
	if (dev->vendor_name) {
		delete dev->vendor_name;
	}
	dev->vendor_name = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::ModelName, charStr);
	if (dev->model_name) {
		delete dev->model_name;
	}
	dev->model_name = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::FirmwareRevision, charStr);
	if (dev->firmware_rev) {
		delete dev->firmware_rev;
	}
	dev->firmware_rev = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::ApplicationSoftwareVersion, charStr);
	if (dev->application_software_ver) {
		delete dev->application_software_ver;
	}
	dev->application_software_ver = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::Description, charStr);
	if (dev->device_description) {
		delete dev->device_description;
	}
	dev->device_description = (frString *)VsbConverter::toString(charStr);
	server->getProperty(PropertyIdentifierEnum::Location, charStr);
	if (dev->device_location) {
		delete dev->device_location;
	}
	dev->device_location = (frString *)VsbConverter::toString(charStr);
	if (dev->device_profile) {
		delete dev->device_profile;
	}
	server->getProperty(PropertyIdentifierEnum::ProfileName, charStr);
	dev->device_profile = (frString *)VsbConverter::toString(charStr);
}

dword bpublic fraGetNumberOfObjects(void) {
	DeviceRef device = StackAccessor::getDevice(*ServerManager::begin()->second);
	return (dword)device->getCount();
}

dword bpublic fraGetNextObject(dword oid) {
	DeviceRef device = StackAccessor::getDevice(*ServerManager::begin()->second);
	ObjectRef obj;
	if (oid == noobject) {
		obj = device->getNextObject(0);
	} else {
		ObjectIdentifier bacOid(oid);
		obj = device->getNextObject(&bacOid);
	}
	return (dword)(obj ? obj->getOid() : noobject);
}

bool  bpublic fraGetTimeDate(frTimeDate *dt) {
	DateTime today;
	VsbConverter::toDateTime(today, *dt);
	return true;
}


void  bpublic fraIncrementDatabaseRevision() {

}

void  bpublic fraLog(char *fmt, ...) {
	std::string str;
	va_list args;
    va_start(args, fmt);
    FC::StringVPrintf(str, fmt, args);
    va_end(args);
    FC_Debug1(str);
}

bool  bpublic fraMSTPTxComplete(void) {
	return false;
}

void  bpublic fraPrivateTransfer(frSource *,word,byte *,word) {

}

int bpublic fraWriteProperty(dword oid, dword pid, dword aidx, frVbag *vp) {
	int result = 0;
	try {
		// convert the stack value to BACnet value
		BacnetValueRef val = VsbConverter::fromVbag(*vp);
		if (val) {
			WritePropertyRequest request(ObjectIdentifier(oid), (PropertyIdentifierEnum::Enum)pid,
					*val, vp->priority, aidx);
			WritePropertyAckRef ack;
			StackAccessor::handleConfirmedRequest(*ServerManager::begin()->second, request, ack);
			if (!ack) {
				result = VsbConverter::toError(Error(ErrorClassEnum::Services, ErrorCodeEnum::Other));
			}
		} else {
			result = VsbConverter::toError(Error(ErrorClassEnum::Property,
								ErrorCodeEnum::DatatypeNotSupported));
		}
	} catch(BacnetErrorException& ex) {
		result = VsbConverter::toError(Error(ex.eClass(), ex.eCode()));
	}
	return result;
}

int   bpublic fraReadProperty(dword oid, dword pid, dword aidx, frVbag *vp, dword *nextpid) {
	int result = 0;
	try {
		ReadPropertyRequest request(ObjectIdentifier(oid), (PropertyIdentifierEnum::Enum)pid, aidx);
		ReadPropertyAckRef ack;
		StackAccessor::handleConfirmedRequest(*ServerManager::begin()->second, request, ack);
		if (ack) {
			if (!VsbConverter::toVbag(*(ack->value()), *vp)) {
				result = VsbConverter::toError(Error(ErrorClassEnum::Property,
						ErrorCodeEnum::InvalidDataType));
			}
		} else {
			result = VsbConverter::toError(Error(ErrorClassEnum::Services, ErrorCodeEnum::Other));
		}
	} catch(BacnetErrorException& ex) {
		result = VsbConverter::toError(Error(ex.eClass(), ex.eCode()));
	}
	return result;
}


int   bpublic fraReinitDevice(int, frString *) {
	return 0;
}

void  bpublic fraResponse(frVbag *bag) {
	ServerRef server = ServerManager::begin()->second;
	TransactionRef trans = StackAccessor::getTransactionHandle(*ServerManager::begin()->second, *bag);
	if (trans) {
		trans->resetCompleteTime();
		StackAccessor::handleConfirmedAck(*server, *trans);
	}
}

void  bpublic fraSaveDeviceVars(frDevInfo *) {

}

bool  bpublic fraSetTimeDate(frTimeDate *) {
	return false;
}


int   bpublic fraSubscribeCOV (frSubscribeCOV *) {
	return 0;
}

void  bpublic fraUnsubscribedCOV(frCOV *) {

}

void  bpublic fraWhoHas(word snet, bool byname, dword objid, frString *oname) {
	DeviceRef device = StackAccessor::getDevice(*ServerManager::begin()->second);
	ObjectRef robj;
	if (byname) {
		if (oname) {
			CharacterString str = VsbConverter::fromString(*oname);
			robj = device->getObject(str.get());
		}
	} else {
		robj = device->getObject(ObjectIdentifier(objid));
	}
	if (robj) {
		VsbString vsbStr;
		frTransmitIHave(snet, robj->getOid(),
				(frString*)VsbConverter::toString(CharacterString(robj->name()), vsbStr));
	}
}

/// No support for mstp physical layer
int bpublic mstpSerialRx() {
	return -1;
}

bool  bpublic mstpSerialTxBuf(byte *b,word n) {
	return false;									//success
}


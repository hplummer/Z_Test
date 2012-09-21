/*
 * BacnetServer.h
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetServer_h
#define BacnetServer_h

#include "fc.h"
#include "BacnetDevice.h"
#include "BacnetUnconfirmedServices.h"
#include "BacnetConfirmedServices.h"
#include "BacnetConfirmedServicesAck.h"
#include "vsbhp.h"

namespace VIGBACNET {

const uint32_t REQUEST_TIMEOUT = 3; // in seconds
const uint32_t REQUEST_RETRIES = 2;

struct PropertiesSetter {
	ObjectInstance deviceInstance;
	std::string deviceName;
	DeviceStatus systemStatus;
	std::string vendorName;
	uint32_t vendorId;
	std::string modelName;
	std::string firmwareRevision;
	std::string softwareVersion;
	std::string location;
	std::string description;
	uint32_t apduTimeout;
	uint32_t apduRetries;
	uint32_t databaseRevision;
	std::string bbmdIp;
	uint16_t bbmdTtl;
	DeviceAddress address;
	std::string broadcastAddress;
	unsigned int processRate;
};

extern PropertiesSetter defaultPropertiesSetter;

class Server;
typedef FC::Ref<Server> ServerRef;

class ServerManager {
public:
	static const size_t MaxServerAllowed = 1;
	typedef std::map<ObjectInstance, ServerRef> InstanceServerMap;
	typedef InstanceServerMap::iterator iterator;

	static ServerRef createServer(const PropertiesSetter& = defaultPropertiesSetter);
	static bool deleteServer(ObjectInstance);
	static ServerRef getServer(ObjectInstance);
	static size_t totalServer() { return _servers.size(); }
	static iterator begin() { return _servers.begin(); }
	static iterator end() { return _servers.end(); }

private:
	static void setServerProperties(const ServerRef&, const PropertiesSetter&);
	static InstanceServerMap _servers;
};

/**
 * Define a bacnet transaction
 * A transaction creates a VBag for any stack bacnet transaction and is identifed
 * by an {id} given by the client app.  It tracks when the transaction was created
 * and is desired when it was completed.  It also translate the stack transaction
 * state to a server transaction state.
 */
class Transaction : public virtual FC::RefObject {
public:
	typedef unsigned long long IdType;

	/**
	 * Define the different possible state for a transaction
	 * Dead means that the transaction cannot be used anymore
	 */
	enum State {
		Idle,
		Pending,
		Complete,
		Dead,
	};

	Transaction(const IdType& transId, const ConfirmedServiceChoiceEnum& service,
			ConfirmedRequestAckRef ack = 0) :
		_create(time(0)), _complete(0), _transId(transId), _service(service), _ack(ack) {
		_bag = new frVbag;
	}

	~Transaction() {
		delete _bag;
	}

	void resetCompleteTime() {
		if (state() == Complete) {
			_complete = time(0);
		}
	}
	void resetCreateTime() { _create= time(0); }

	frVbag* vbag() const { return _bag; }
	IdType transId() const { return _transId; }
	time_t createTime() const { return _create; }
	time_t completeTime() const { return _complete; }
	const ConfirmedServiceChoiceEnum& service() const { return _service; }
	ConfirmedRequestAckRef ack() const { return _ack; }
	State state() const {
		switch (vbag()->status) {
		case vbsIdle:
			return Idle;
		case vbsPending:
			return Pending;
		case vbsComplete:
			return Complete;
		}
		return Dead;
	}

	bool isSimpleAck() const {
		return _bag->pdtype == adtSACK;
	}

	bool hasError() const {
		return _bag->pdtype == adtError;
	}

private:
	time_t _create;
	time_t _complete;
	IdType _transId;
	frVbag* _bag;
	ConfirmedServiceChoiceEnum _service;
	ConfirmedRequestAckRef _ack;
};
typedef FC::Ref<Transaction> TransactionRef;

/**
 * Manages client transaction request
 * This class create transactions for a new client request and
 * bind it to an unique invokeid.  A worker method needs to be called periodically
 * to do some cleanup.  A completed transaction will be kept live for about
 * 5s afterward it will be destroyed.
 */
class TransactionManager : public virtual FC::RefObject {
public:
	static const time_t RecycleTime = 320;
	static const time_t LiveTime = 5;

	TransactionManager() : _currentId(0) {}

	TransactionRef createTransaction(const ConfirmedServiceChoiceEnum&, ConfirmedRequestAckRef = 0);
	void deleteTransaction(TransactionRef);
	void deleteTransaction(const Transaction::IdType&);
	void deleteTransaction(const frVbag&);
	TransactionRef getTransaction(const Transaction::IdType&);
	TransactionRef getTransaction(const frVbag&);
	Transaction::State getState(const Transaction::IdType&) const;
	ConfirmedRequestAckRef getAck(const Transaction::IdType&);
	void extendTransactionLife(const Transaction::IdType&);
	void cleanup();

private:
	std::map<Transaction::IdType, TransactionRef > _uuidTransMap;
	std::map<frVbag*, TransactionRef > _vbagTransMap;
	Transaction::IdType _currentId;
};

class ReadRequestEvent : public FC::Event {
public:
	ReadRequestEvent(const ReadPropertyRequest& req) :
		_req(req) {
	}

	const ReadPropertyRequest& request() const { return _req; }

private:
	ReadPropertyRequest _req;
};

class WriteRequestEvent : public FC::Event {
public:
	WriteRequestEvent(const WritePropertyRequest& req) :
		_req(req) {
	}

	const WritePropertyRequest& request() const { return _req; }

private:
	WritePropertyRequest _req;
};


class ResponseEvent : public FC::Event {
public:
	ResponseEvent(Transaction::IdType id) :
		_transId(id){
	}

	Transaction::IdType transactionId() const { return _transId; }

private:
	Transaction::IdType _transId;
};

class ReadAckEvent : public ResponseEvent {
public:
	ReadAckEvent(Transaction::IdType id, const ReadPropertyAck& ack) :
		ResponseEvent(id), _ack(ack) {
	}

	const ReadPropertyAck& ack() const { return _ack; }

private:
	ReadPropertyAck _ack;
};

class WriteAckEvent : public ResponseEvent {
public:
	WriteAckEvent(Transaction::IdType id, const WritePropertyAck& ack) :
		ResponseEvent(id), _ack(ack) {
	}

	const WritePropertyAck& ack() const { return _ack; }

private:
	WritePropertyAck _ack;
};

class ErrorEvent : public ResponseEvent {
public:
	ErrorEvent(Transaction::IdType id, const Error error) :
		ResponseEvent(id), _error(error) {
	}

	const Error& error() const { return _error; }

private:
	Error _error;
};

class IAmEvent : public FC::Event {
public:
	IAmEvent(const IAmRequest& request) :
		_request(request) {
	}

	const IAmRequest& request() const { return _request; }

private:
	IAmRequest _request;
};



class Server : public FC::EventThread {
public:
	static const byte DoWorkRate = 5; // How often we need to call do work on the stack in msec
	static const unsigned MaxRequest = 256;

	friend class ServerManager;
	friend class StackAccessor;

	ObjectInstance getInstance() const {
		return _localDev->getInstance();
	}

	std::string getName() const {
		return _localDev->getName();
	}

	unsigned getMaxRequest() const { return MaxRequest; }

	DeviceAddress getAddress() const {
		return _localDev->getAddress();
	}

	std::string getBbmdIp() const {
		return _bbmdIp;
	}

	uint16_t getBbmdTtl() const {
		return _bbmdTtl;
	}

	std::string getBroadcast() const {
		return _broadcast;
	}

	void addObject(const Object& obj) {
		FC::Mutex(_mutex);
		_localDev->addObject(obj);
	}

	void deleteObject(const ObjectIdentifier& oid) {
		FC::Mutex(_mutex);
		_localDev->deleteObject(oid);
	}

	uint32_t getNextObjectInstance(const ObjectTypeEnum& type) const {
		FC::Mutex(_mutex);
		return _localDev->getNextObjectInstance(type);
	}

	template <typename T>
	bool getProperty(const PropertyIdentifierEnum& id, T& value, bool throwError = true) const {
		FC::Mutex(_mutex);
		return _localDev->getProperty(id, value, throwError);
	}
	template <typename T>
	bool getProperty(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id,
			T& value, bool throwError = true) const {
		FC::Mutex(_mutex);
		return _localDev->getObjectProperty(oid, id, value, throwError);
	}
	template <typename T>
	bool setProperty(const PropertyIdentifierEnum& id, const T& value, bool throwError = true) {
		FC::Mutex(_mutex);
		return _localDev->setProperty(id, value, throwError);
	}
	template <typename T>
	bool setProperty(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id,
			const T& value, bool throwError = true) {
		FC::Mutex(_mutex);
		return _localDev->setObjectProperty(oid, id, value, throwError);
	}
	bool isPropertyRemoteWrittable(const PropertyIdentifierEnum& id) const {
		FC::Mutex(_mutex);
		return _localDev->isPropertyRemoteWrittable(id);
	}
	bool isPropertyRemoteWrittable(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id) {
		FC::Mutex(_mutex);
		return _localDev->isPropertyRemoteWrittable(oid, id);
	}
	bool isPropertyModified(const PropertyIdentifierEnum& id) const {
		FC::Mutex(_mutex);
		return _localDev->isPropertyModified(id);
	}
	bool isPropertyModified(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id) {
		FC::Mutex(_mutex);
		return _localDev->isPropertyModified(oid, id);
	}
	void clearPropertyModified(const PropertyIdentifierEnum& id) const {
		FC::Mutex(_mutex);
		_localDev->clearPropertyModified(id);
	}
	void clearPropertyModified(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id) {
		FC::Mutex(_mutex);
		_localDev->clearPropertyModified(oid, id);
	}
	bool isPropertyDirty(const PropertyIdentifierEnum& id) const {
		FC::Mutex(_mutex);
		return _localDev->isPropertyDirty(id);
	}
	bool isPropertyDirty(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id) {
		FC::Mutex(_mutex);
		return _localDev->isPropertyDirty(oid, id);
	}
	void clearPropertyDirty(const PropertyIdentifierEnum& id) const {
		FC::Mutex(_mutex);
		_localDev->clearPropertyDirty(id);
	}
	void clearPropertyDirty(const ObjectIdentifier& oid, const PropertyIdentifierEnum& id) {
		FC::Mutex(_mutex);
		_localDev->clearPropertyDirty(oid, id);
	}

	bool hasObject(const ObjectIdentifier& oid) const {
		FC::Mutex(_mutex);
		return _localDev->hasObject(oid);
	}

	void addRemoteDevice(const Device& device) {
		FC::Mutex(_mutex);
		_remoteDev[device.getInstance()] = new Device(device);
	}

	void deleteRemoteDevice(ObjectInstance devInstance) {
		FC::Mutex(_mutex);
		_remoteDev.erase(devInstance);
	}

	bool knowsRemoteDevice(ObjectInstance devInstance) const {
		FC::Mutex(_mutex);
		return _remoteDev.find(devInstance) != _remoteDev.end();
	}

	template <typename T>
	bool getRemoteProperty(ObjectInstance devInstance, const ObjectIdentifier& oid, T& value,
			bool throwError = true) const {
		FC::Mutex(_mutex);
		auto it = _remoteDev.find(devInstance);
		if (it) {
			return it->second->getObjectProperty(oid, value, throwError);
		} else if (throwError) {
			throwException(BacnetErrorException(ErrorClassEnum::Communication,
						ErrorCodeEnum::UnknownDevice,
						FC::StringAPrintf("No device with instance %d exists.", devInstance)));
		}
		return false;
	}

	template <typename T>
	bool setRemoteProperty(ObjectInstance devInstance, const ObjectIdentifier& oid, T& value,
			bool throwError = true) const {
		FC::Mutex(_mutex);
		auto it = _remoteDev.find(devInstance);
		if (it) {
			return it->second->setObjectProperty(oid, value, throwError);
		} else if (throwError) {
			throwException(BacnetErrorException(ErrorClassEnum::Communication,
						ErrorCodeEnum::UnknownDevice,
						FC::StringAPrintf("No device with instance %d exists.", devInstance)));
		}
		return false;
	}

	// Bacnet Service Public API
	void sendWhoIs(const WhoIsRequest&) const;
	void sendIHave() const;
	void sendIAm() const;
	Transaction::IdType sendReadProperty(ObjectInstance, const ReadPropertyRequest&) const;
	Transaction::IdType sendWriteProperty(ObjectInstance, const WritePropertyRequest&) const;


	// Transaction Public API
	Transaction::State getTransactionState(const Transaction::IdType&) const;
	BacnetValueRef getTransactionValue(const Transaction::IdType&) const;
	bool isTransactionSimpelAck(const Transaction::IdType&) const;
	bool isTransactionError(const Transaction::IdType&) const;
	void deleteTransaction(const Transaction::IdType&) const;

	std::string toString(std::string *str);

protected:
	class DoWorkTimer : public FC::TimerEvent {
	public:
		DoWorkTimer(unsigned doWorkRate_msec) :
		FC::TimerEvent(0, doWorkRate_msec){
		}
	};

	void setAddress(const DeviceAddress& address) {
		_localDev->setAddress(address);
	}

	void setBbmdIp(const std::string &ip) {
		_bbmdIp = ip;
	}

	void setBbmdTtl(uint16_t ttl) {
		_bbmdTtl = ttl;
	}

	void setBroadcast(const std::string &ip) {
		_broadcast = ip;
	}

	TransactionRef getTransactionHandle(const Transaction::IdType&) const;
	TransactionRef getTransactionHandle(const frVbag&) const;

	virtual void initialize();
	virtual void fini();

	void onDoWork(DoWorkTimer*);
	void doWork();

	template <typename UNC_REQUEST>
	void handleUnconfirmedRequest(const DeviceAddress&, const UNC_REQUEST&) {
		return; // Ignore all the request we do not know
	}
	/// Look for template specialization in the cpp file for handling Unconfirmed requests:
	///	- I AM

	template <typename CONF_REQUEST, typename CONF_ACK>
	void handleConfirmedRequest(const CONF_REQUEST& request, FC::Ref<CONF_ACK>&) {
		// throw unsupported service for all unknown request
		throw BacnetErrorException(ErrorClass::Services, ErrorCode::ServiceRequestDenied,
				FC::StringAPrintf("Service %s is not supported by this device", request.service().name()));
	}
	/// Look for template specialization in the cpp file for handling Confirmed requests:
	/// - ReadProperty
	/// - WriteProperty

	void handleConfirmedRequestAck(const Transaction&);
	void handleReadAck(const Transaction&);
	void handleWriteAck(const Transaction&);

	template<typename EVENT, typename REQ>
	void notifyRequest(const REQ& req) {
		FC::Ref<EVENT> event(new EVENT(req));
		post(event);
	}

	template<typename EVENT, typename ACK>
	void notifyAck(const Transaction::IdType& id, const ACK& ack) {
		FC::Ref<EVENT> event(new EVENT(id, ack));
		post(event);
	}

private:
	Server(ObjectInstance instance, const std::string& name, unsigned doWorkRateMsec = DoWorkRate);

	typedef std::map<ObjectInstance, DeviceRef > DeviceMap;

	DeviceRef _localDev;
	DeviceMap _remoteDev;
	std::string _bbmdIp;
	uint16_t _bbmdTtl;
	std::string _broadcast;
	bool _started;
	unsigned _workRate;
	FC::Ref<TransactionManager> _transMgr;
	struct timeval _lastWork;
	FC::Ref<FC::TimerEvent> _workTimer;
	FC::Mutex _mutex;
};



} // VIGBACNET




#endif /* BACNETSERVER_H_ */

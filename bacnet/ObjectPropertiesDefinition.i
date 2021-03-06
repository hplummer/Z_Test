/*
START_OBJECT(objectType, supported)
	PROPERTY(PropId, isRequired, isRemoteWrittable, new BacnetValue)
	PROPERTY(PropId, isRequired, isRemoteWrittable, BacnetValue)
END_OBJECT
*/

START_OBJECT(ObjectTypeEnum::AnalogInput, true)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogInput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogInput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, false, new Real())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Units, true, false, new Units())
END_OBJECT

START_OBJECT(ObjectTypeEnum::AnalogOutput, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new Real())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Units, true, false, new Units())
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new Real(0))
END_OBJECT

START_OBJECT(ObjectTypeEnum::AnalogValue, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new Real())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags());
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Units, true, false, new Units());
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new Real(0))
END_OBJECT

START_OBJECT(ObjectTypeEnum::BinaryInput, true)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogInput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogInput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, false, new BinaryPV())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Polarity, true, false, new Polarity())
END_OBJECT

START_OBJECT(ObjectTypeEnum::BinaryOutput, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new BinaryPV())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Polarity, true, false, new Polarity())
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new BinaryPV(BinaryPV::Inactive))
END_OBJECT

START_OBJECT(ObjectTypeEnum::BinaryValue, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new BinaryPV())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::Polarity, true, false, new Polarity())
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new BinaryPV(BinaryPV::Inactive))
END_OBJECT

START_OBJECT(ObjectTypeEnum::Device, true)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::Device, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::Device))
	PROPERTY(PropertyIdentifierEnum::SystemStatus, true, false, new DeviceStatus())
	PROPERTY(PropertyIdentifierEnum::VendorName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::VendorIdentifier, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::ModelName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::FirmwareRevision, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ApplicationSoftwareVersion, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::Location, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ProtocolVersion, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::ProtocolRevision, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::ProtocolServicesSupported, true, false, new ServicesSupported())
	PROPERTY(PropertyIdentifierEnum::ProtocolObjectTypesSupported, true, false, new ObjectTypesSupported())
	PROPERTY(PropertyIdentifierEnum::ObjectList, true, false, 0)
	PROPERTY(PropertyIdentifierEnum::MaxApduLengthAccepted, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::SegmentationSupported, true, false, new Segmentation())
	PROPERTY(PropertyIdentifierEnum::LocalTime, false, false, new Time())
	PROPERTY(PropertyIdentifierEnum::LocalDate, false, false, new Date())
	PROPERTY(PropertyIdentifierEnum::UtcOffset, false, false, new Integer())
	PROPERTY(PropertyIdentifierEnum::DaylightSavingsStatus, false, false, new Boolean())
	PROPERTY(PropertyIdentifierEnum::ApduSegmentTimeout, false, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::ApduTimeout, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::NumberOfApduRetries, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::DeviceAddressBinding, true, false, 0)
	PROPERTY(PropertyIdentifierEnum::DatabaseRevision, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::ProfileName, false, false, new CharacterString())
END_OBJECT

START_OBJECT(ObjectTypeEnum::MultistateInput, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, false, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::NumberOfStates, true, false, new Unsigned())
END_OBJECT

START_OBJECT(ObjectTypeEnum::MultistateOutput, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::NumberOfStates, true, false, new Unsigned())
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new BinaryPV(BinaryPV::Inactive))
END_OBJECT

START_OBJECT(ObjectTypeEnum::MultistateValue, false)
	PROPERTY(PropertyIdentifierEnum::ObjectIdentifier, true, false, new ObjectIdentifier(ObjectTypeEnum::AnalogOutput, ObjectIdentifier::MaxInstance))
	PROPERTY(PropertyIdentifierEnum::ObjectName, true, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::ObjectType, true, false, new ObjectType(ObjectTypeEnum::AnalogOutput))
	PROPERTY(PropertyIdentifierEnum::PresentValue, true, true, new Unsigned())
	PROPERTY(PropertyIdentifierEnum::Description, false, false, new CharacterString())
	PROPERTY(PropertyIdentifierEnum::StatusFlags, true, false, new StatusFlags())
	PROPERTY(PropertyIdentifierEnum::EventState, true, false, new EventState())
	PROPERTY(PropertyIdentifierEnum::OutOfService, true, false, new Boolean(false))
	PROPERTY(PropertyIdentifierEnum::NumberOfStates, true, false, new Unsigned())
	//PROPERTY(PropertyIdentifierEnum::PriorityArray, true, false, new PriorityArray())
	PROPERTY(PropertyIdentifierEnum::RelinquishDefault, true, false, new BinaryPV(BinaryPV::Inactive))
END_OBJECT
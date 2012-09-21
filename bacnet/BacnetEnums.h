/*
 * BacnetEnums.h
 *
 *  Created on: Aug 6, 2012
 * 
 * Copyright (c) 2012 Vigilent Corporation.  All Rights Reserved.
 */

#ifndef BacnetEnums_h
#define BacnetEnums_h

#include "fc.h"

namespace VIGBACNET {

template <typename T>
struct EnumTemplate : public virtual T, public virtual FC::Formatter {
	typedef typename T::Enum Enum;

	EnumTemplate() :
		_e(T::getDefault()) {}

	EnumTemplate(const Enum& e) :
		_e(e) {}

	const Enum get() const {
		return _e;
	}

	const char* name() const {
		return T::getName(_e);
	}

	EnumTemplate& operator=(const Enum& e) {
		_e = e;
		return *this;
	}

	Enum operator()() {
		return _e;
	}

	bool operator==(const EnumTemplate::Enum& e) const {
		return _e == e;
	}

	bool operator!=(const EnumTemplate::Enum& e) const {
		return _e != e;
	}

	bool operator>(const EnumTemplate::Enum& e) const {
		return _e > e;
	}

	bool operator>=(const EnumTemplate::Enum& e) const {
		return _e >= e;
	}

	bool operator<(const EnumTemplate::Enum& e) const {
		return _e < e;
	}

	bool operator<=(const EnumTemplate::Enum& e) const {
		return _e <= e;
	}

	operator Enum() const {
		return _e;
	}

	operator const char*() const {
		return name();
	}

	operator std::string() const {
		return name();
	}

	virtual void out(std::ostream& os) const {
		os << name();
	}

private:
	Enum _e;
};

struct MonthEnumDefinition {
	enum Enum {
		January = 1,
		February,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December,
		Odd_Months,
		Even_Months,
		Unspecified_Month
	};

	static const char* getTypeName() {
		return "Month";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case January:
			return "January";
		case February:
			return "February";
		case March:
			return "March";
		case April:
			return "April";
		case May:
			return "May";
		case June:
			return "June";
		case July:
			return "July";
		case August:
			return "August";
		case September:
			return "September";
		case October:
			return "October";
		case November:
			return "November";
		case December:
			return "December";
		case Odd_Months:
			return "Odd Months";
		case Even_Months:
			return "Even Months";
		case Unspecified_Month:
			return "Unspecified Month";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Unspecified_Month;
	}
};
typedef EnumTemplate<MonthEnumDefinition> MonthEnum;


struct DayOfTheWeekEnumDefinition {
	enum Enum {
		Monday = 1,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday,
		Unspecified_Day
	};

	static const char* getTypeName() {
		return "Day Of The Week";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Monday:
			return "Monday";
		case Tuesday:
			return "Tuesday";
		case Wednesday:
			return "Wednesday";
		case Thursday:
			return "Thursday";
		case Friday:
			return "Friday";
		case Saturday:
			return "Saturday";
		case Sunday:
			return "Sunday";
		case Unspecified_Day:
			return "Unspecified Day";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Unspecified_Day;
	}
};
typedef EnumTemplate<DayOfTheWeekEnumDefinition> DayOfTheWeekEnum;

struct CharsetEnumDefinition {
	enum Enum {
		ANSI_X34,
		MS_DBCS,
		JIS_6226,
		UCS4,
		UCS2,
		ISO8859
	};

	static const char* getTypeName() {
		return "Charset";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case ANSI_X34:
			return "ANSI X34";
		case MS_DBCS:
			return "Microsoft DBCS";
		case JIS_6226:
			return "JIS C 6226";
		case UCS4:
			return "ISO 10646 (UCS4)";
		case UCS2:
			return "ISO 10646 (UCS2)";
		case ISO8859:
			return "ISO 8859-1";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return ANSI_X34;
	}
};
typedef EnumTemplate<CharsetEnumDefinition> CharsetEnum;

struct DataTypeEnumDefinition {
	enum Enum {
		/// First data type are the official BACnet Primitive Type tag number
		Null = 0,
		Boolean = 1,
		Unsigned = 2,
		Integer = 3,
		Real = 4,
		Double = 5,
		OctetString = 6,
		CharacterString = 7,
		BitString = 8,
		Enumerated = 9,
		Date = 10,
		Time = 11,
		ObjectIdentifier = 12,
		/// BACnet Base Type.  Id are not known from BACnet
		Unknown = 255
	};

	static const char *getTypeName() {
		return "ObjectType";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Null:
			return "Null";
		case Boolean:
			return "Boolean";
		case Unsigned:
			return "Unsigned";
		case Integer:
			return "Integer";
		case Real:
			return "Real";
		case Double:
			return "Double";
		case OctetString:
			return "Octet String";
		case CharacterString:
			return "Character String";
		case BitString:
			return "Bit String";
		case Enumerated:
			return "Enumerated";
		case Date:
			return "Date";
		case Time:
			return "Time";
		case ObjectIdentifier:
			return "Object Identifier";
		case Unknown:
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Null;
	}

	Enum _e;
};
typedef EnumTemplate<DataTypeEnumDefinition> DataTypeEnum;




struct ObjectTypeEnumDefinition {

	enum Enum {
		AnalogInput = 0,
		AnalogOutput = 1,
		AnalogValue  =2,
		BinaryInput = 3,
		BinaryOutput = 4,
		BinaryValue = 5,
		Device = 8,
		MultistateInput = 13,
		MultistateOutput = 14,
		MultistateValue = 19
	};

	static const char *getTypeName() {
		return "ObjectType";
	}

	static const char* getName(Enum e) {
		switch(e) {
		case AnalogInput:
			return "Analog Input";
		case AnalogOutput:
			return "Analog Output";
		case AnalogValue:
			return "Analog Value";
		case BinaryInput:
			return "Binary Input";
		case BinaryOutput:
			return "Binary Output";
		case BinaryValue:
			return "Binary Value";
		case Device:
			return "Device";
		case MultistateInput:
			return "Multistate Input";
		case MultistateOutput:
			return "Multistate Output";
		case MultistateValue:
			return "Multistate Value";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return AnalogInput;
	}

	Enum _e;
};
typedef EnumTemplate<ObjectTypeEnumDefinition> ObjectTypeEnum;


struct PropertyIdentifierEnumDefinition {
	enum Enum {
		AckedTransition = 0,
		AckRequiredId = 1,
		ActiveText = 4,
		AlarmValue = 6,
		All = 8,
		ApduSegmentTimeout = 10,
		ApduTimeout = 11,
		ApplicationSoftwareVersion = 12,
		Archive = 13,
		DatabaseRevision = 155,
		DaylightSavingsStatus = 24,
		Deadband = 25,
		Description = 28,
		DeviceAddressBinding = 30,
		EventTable = 35,
		EventState = 36,
		EventTimeStamps = 130,
		FileAccessMethod = 41,
		FileSize = 42,
		FileType = 43,
		FirmwareRevision = 44,
		HighLimit = 45,
		InactiveText = 46,
		IssueConfirmedNotifications = 51,
		LastRestoreTime = 157,
		LimitEnable = 52,
		LocalDate = 56,
		LocalTime = 57,
		Location = 58,
		LowLimit = 59,
		MaxApduLengthAccepted = 62,
		MaxInfoFrames = 63,
		MaxMaster = 64,
		ModelName = 70,
		ModificationDate = 71,
		NotifyType = 72,
		NumberOfApduRetries = 73,
		NumberOfStates = 74,
		ObjectIdentifier = 75,
		ObjectList = 76,
		ObjectName = 77,
		ObjectType = 79,
		Optional = 80,
		OutOfService = 81,
		Polarity = 84,
		PresentValue = 85,
		Priority = 86,
		PriorityArray = 87,
		ProcessIdentifier = 89,
		ProfileName = 168,
		ProtocolConformanceClass = 95,
		ProtocolObjectTypesSupported = 96,
		ProtocolServicesSupported = 97,
		ProtocolVersion = 98,
		ReadOnly = 99,
		Recipient = 101,
		RecordCount = 141,
		Reliability = 103,
		RelinquishDefault = 104,
		Required = 105,
		SegmentationSupported = 107,
		StateText = 110,
		StatusFlags = 111,
		SystemStatus = 112,
		TimeDelay = 113,
		Units = 117,
		UtcOffset = 119,
		VendorIdentifier = 120,
		VendorName = 121,
		ProtocolRevision = 139
	};

	static const char *getTypeName() {
		return "PropertyIdentifier";
	}

	static const char* getName(Enum e) {
		switch(e) {
		case AckedTransition:
			return "Acked Transition";
		case AckRequiredId:
			return "Ack Required Id";
		case ActiveText:
			return "Active Text";
		case AlarmValue:
			return "Alarm Value";
		case All:
			return "All";
		case ApduSegmentTimeout:
			return "Apdu Segment Timeout";
		case ApduTimeout:
			return "Apdu Timeout";
		case ApplicationSoftwareVersion:
			return "Application Software Version";
		case Archive:
			return "Archive";
		case DatabaseRevision:
			return "Database Version";
		case DaylightSavingsStatus:
			return "Daylight Saving Status";
		case Deadband:
			return "Deadband";
		case Description:
			return "Description";
		case DeviceAddressBinding:
			return "Device Address Binding";
		case EventTable:
			return "Event Table";
		case EventState:
			return "Event State";
		case EventTimeStamps:
			return "Event Time Stamps";
		case FileAccessMethod:
			return "File Access Method";
		case FileSize:
			return "File Size";
		case FileType:
			return "File Type";
		case FirmwareRevision:
			return "Firmware Version";
		case HighLimit:
			return "High Limit";
		case InactiveText:
			return "Inactive Text";
		case IssueConfirmedNotifications:
			return "Issue Confirmed Notifications";
		case LastRestoreTime:
			return "Last Restore Time";
		case LimitEnable:
			return "Limit Enable";
		case LocalDate:
			return "Local Date";
		case LocalTime:
			return "Local Time";
		case Location:
			return "Location";
		case LowLimit:
			return "Low Limit";
		case MaxApduLengthAccepted:
			return "Max Apdu Length Accepted";
		case MaxInfoFrames:
			return "Max Info Frames";
		case MaxMaster:
			return "Max Master";
		case ModelName:
			return "Model Name";
		case ModificationDate:
			return "Modification Date";
		case NotifyType:
			return "Notify Type";
		case NumberOfApduRetries:
			return "Number Of Apdu Retries";
		case NumberOfStates:
			return "Number Of States";
		case ObjectIdentifier:
			return "Object Identifier";
		case ObjectList:
			return "Object List";
		case ObjectName:
			return "Object Name";
		case ObjectType:
			return "Object Type";
		case Optional:
			return "Optional";
		case OutOfService:
			return "Out Of Service";
		case Polarity:
			return "Polarity";
		case PresentValue:
			return "Present Value";
		case Priority:
			return "Priority";
		case PriorityArray:
			return "Priority Array";
		case ProcessIdentifier:
			return "Process Identifier";
		case ProfileName:
			return "Profile Name";
		case ProtocolConformanceClass:
			return "Protocol Conformance Class";
		case ProtocolObjectTypesSupported:
			return "Protocol Object Types Supported";
		case ProtocolServicesSupported:
			return "Protocol Services Supported";
		case ProtocolVersion:
			return "Protocol Version";
		case ReadOnly:
			return "Read Only";
		case Recipient:
			return "Recipient";
		case RecordCount:
			return "Record Count";
		case Reliability:
			return "Reliability";
		case RelinquishDefault:
			return "Relinquish Default";
		case Required:
			return "Required";
		case SegmentationSupported:
			return "Segmentation Supported";
		case StateText:
			return "State Text";
		case StatusFlags:
			return "Status Flags";
		case SystemStatus:
			return "System Status";
		case TimeDelay:
			return "Time Delay";
		case Units:
			return "Units";
		case UtcOffset:
			return "UtcOffset";
		case VendorIdentifier:
			return "Vendor Identifier";
		case VendorName:
			return "Vendor Name";
		case ProtocolRevision:
			return "Protocol Revision";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return PresentValue;
	}
};
typedef EnumTemplate<PropertyIdentifierEnumDefinition> PropertyIdentifierEnum;

struct ConfirmerdServiceChoiceEnumDefinition {
	enum Enum {
	// Alarm and Event Services
		AcknowledgeAlarm = 0,
		ConfirmedCOVNotification = 1,
		ConfirmedEventNotification = 2,
		GetAlarmSummary = 3,
		GetEnrollmentSummary = 4,
		GetEventInformation = 29,
		SubscribeCOV = 5,
		SubscribeCOVProperty = 28,
		LifeSafetyOperation = 27,

	// File Access Services
		AtomicReadFile = 6,
		AtomicWriteFile = 7,

	// Object Access Services
		AddListElement = 8,
		RemoveListElement = 9,
		CreateObject = 10,
		DeleteObject = 11,
		ReadProperty = 12,
		ReadPropertyConditional = 13,
		ReadPropertyMultiple = 14,
		ReadRange = 26,
		WriteProperty = 15,
		WritePropertyMultiple = 16,

	// Remote Device Management Services
		DeviceCommunicationControl = 17,
		ConfirmedPrivateTransfer = 18,
		ConfirmedTextMessage = 19,
		ReinitializeDevice = 20,

	// Virtual Terminal Services
		VtOpen = 21,
		VtClose = 22,
		VtData = 23,

	// Security Services
		Authenticate = 24,
		RequestKey = 25,
	};

	static const char* getTypeName() {
		return "Confirmed Service Choice";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case AcknowledgeAlarm:
			return "Acknowledge Alarm";
		case ConfirmedCOVNotification:
			return "Confirmed COV Notification";
		case ConfirmedEventNotification:
			return "Confirmed Event Notification";
		case GetAlarmSummary:
			return "Get Alarm Summary";
		case GetEnrollmentSummary:
			return "Get Enrollment Summary";
		case GetEventInformation:
			return "Get Event Information";
		case SubscribeCOV:
			return "Subscribe COV";
		case SubscribeCOVProperty:
			return "Subscribe COV Property";
		case LifeSafetyOperation:
			return "Life Safety Operation";
		case AtomicReadFile:
			return "Atomic Read File";
		case AtomicWriteFile:
			return "Atomic Write File";
		case AddListElement:
			return "Add List Element";
		case RemoveListElement:
			return "Remove List Element";
		case CreateObject:
			return "Create Object";
		case DeleteObject:
			return "Delete Object";
		case ReadProperty:
			return "Read Property";
		case ReadPropertyConditional:
			return "Read Property Conditional";
		case ReadPropertyMultiple:
			return "Read Property Multiple";
		case ReadRange:
			return "Read Range";
		case WriteProperty:
			return "Write Property";
		case WritePropertyMultiple:
			return "Write Property Multiple";
		case DeviceCommunicationControl:
			return "Device Communication Control";
		case ConfirmedPrivateTransfer:
			return "Confirmed Private Transfer";
		case ConfirmedTextMessage:
			return "Confirmed Text Message";
		case ReinitializeDevice:
			return "Reinitialize Device";
		case VtOpen:
			return "VT Open";
		case VtClose:
			return "VT Close";
		case VtData:
			return "VT Data";
		case Authenticate:
			return "Authenticate";
		case RequestKey:
			return "Request Key";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return ReadProperty;
	}
};
typedef EnumTemplate<ConfirmerdServiceChoiceEnumDefinition> ConfirmedServiceChoiceEnum;

struct UnconfirmerdServiceChoiceEnumDefinition {
	enum Enum {
		IAm	= 0,
		IHave = 1,
		UnconfirmedCOVNotification = 2,
		UnconfirmedEventNotification = 3,
		UnconfirmedPrivateTransfer = 4,
		UnconfirmedTextMessage = 5,
		TimeSynchronization = 6,
		WhoHas = 7,
		WhoIs = 8,
		UtcTimeSynchronization = 9
	};

	static const char* getTypeName() {
		return "Unconfirmed Service Choice";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case IAm:
			return "I Am";
		case IHave:
			return "I Have";
		case UnconfirmedCOVNotification:
			return "Unconfirmed COV Notification";
		case UnconfirmedEventNotification:
			return "Unconfirmed Event Notification";
		case UnconfirmedPrivateTransfer:
			return "Unconfirmed Private Transfer";
		case UnconfirmedTextMessage:
			return "Unconfirmed Text Message";
		case TimeSynchronization:
			return "Time Synchronization";
		case WhoHas:
			return "Who Has";
		case WhoIs:
			return "Who Is";
		case UtcTimeSynchronization:
			return "UTC Time Synchronization";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return IAm;
	}
};
typedef EnumTemplate<UnconfirmerdServiceChoiceEnumDefinition> UnconfirmedServiceChoiceEnum;

struct DeviceStatusEnumDefinition {
	enum Enum {
		Operational,
		OperationalReadOnly,
		DownloadRequired,
		DownloadInProgress,
		NonOperational,
		BackupInProgress
	};

	static const char* getTypeName() {
		return "Device Status";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Operational:
			return "Operational";
		case OperationalReadOnly:
			return "Operational Read Only";
		case DownloadRequired:
			return "Download Required";
		case DownloadInProgress:
			return "Download In Progress";
		case NonOperational:
			return "Non Operational";
		case BackupInProgress:
			return "Backup In Progress";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Operational;
	}
};
typedef EnumTemplate<DeviceStatusEnumDefinition> DeviceStatusEnum;


struct EventStateEnumDefinition {
	enum Enum {
		Normal,
		Fault,
		OffNormal,
		HighLimit,
		LowLimit,
		LifeSafetyAlarm
	};

	static const char* getTypeName() {
		return "Event State";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Normal:
			return "Normal";
		case Fault:
			return "Fault";
		case OffNormal:
			return "Off Normal";
		case HighLimit:
			return "High Limit";
		case LowLimit:
			return "Low Limit";
		case LifeSafetyAlarm:
			return "Life Safety Alarm";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Normal;
	}
};
typedef EnumTemplate<EventStateEnumDefinition> EventStateEnum;


struct UnitsEnumDefinition {
	enum Enum {
	// Electrical
		Milliamperes = 2,
		Amperes = 3,
		Volts = 5,
		Millivolts = 124,
		Kilovolts = 6,
		Megavolts = 7,
	// Energy
		WattHours = 18,
		KilowattHours = 19,
		MegawattHours = 146,
		Btus = 20,
		KiloBtus = 147,
		MegaBtus = 148,
	// Frequency
		Hertz = 27,
		Kilohertz = 129,
		Megahertz = 130,
	// Power
		Milliwatts = 132,
		Watts = 47,
		Kilowatts = 48,
		Megawatts = 49,
		Horsepower = 51,
	// Pressure

	// Temperature
		DegreesCelsius = 62,
		DegreesKelvin = 63,
		DegreesFahrenheit = 64,

	// Other
		NoUnits = 95
	};

	static const char* getTypeName() {
		return "Units";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Milliamperes:
			return "Milliamperes";
		case Amperes:
			return "Amperes";
		case Volts:
			return "Volts";
		case Millivolts:
			return "Millivolts";
		case Kilovolts:
			return "Kilovolts";
		case Megavolts:
			return "Megavolts";
		case WattHours:
			return "Watt Hours";
		case KilowattHours:
			return "Kilowatt Hours ";
		case MegawattHours:
			return "Megawatt Hours";
		case Btus:
			return "Btus";
		case KiloBtus:
			return "Kilo Btus";
		case MegaBtus:
			return "Mega Btus";
		case Hertz:
			return "Hertz";
		case Kilohertz:
			return "Kilohertz";
		case Megahertz:
			return "Megahertz";
		case Milliwatts:
			return "Milliwatts";
		case Watts:
			return "Watts";
		case Kilowatts:
			return "Kilowatts";
		case Megawatts:
			return "Megawatts";
		case Horsepower:
			return "Horsepower";
		case DegreesCelsius:
			return "Degrees Celsius";
		case DegreesKelvin:
			return "Degrees Kelvin";
		case DegreesFahrenheit:
			return "Degrees Fahrenheit";
		case NoUnits:
			return "No Units";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return NoUnits;
	}
};
typedef EnumTemplate<UnitsEnumDefinition> UnitsEnum;


struct PolarityEnumDefinition {
	enum Enum {
		Normal,
		Reverse
	};

	static const char* getTypeName() {
		return "Polarity";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Normal:
			return "Normal";
		case Reverse:
			return "Reverse";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Normal;
	}
};
typedef EnumTemplate<PolarityEnumDefinition> PolarityEnum;


struct BinaryPVEnumDefinition {
	enum Enum {
		Inactive,
		Active
	};

	static const char* getTypeName() {
		return "BinaryPVEnum";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Inactive:
			return "Inactive";
		case Active:
			return "Active";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Inactive;
	}
};
typedef EnumTemplate<BinaryPVEnumDefinition> BinaryPVEnum;


struct SegmentationEnumDefinition {
	enum Enum {
		SegmentedBoth,
		SegmentedTransmit,
		SegmentedReceive,
		NoSegmentation
	};

	static const char* getTypeName() {
		return "Segmentation";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case SegmentedBoth:
			return "Segmented Both";
		case SegmentedTransmit:
			return "Segmented Transmit";
		case SegmentedReceive:
			return "Segmented Receive";
		case NoSegmentation:
			return "NoSemgentation";
		}
	}

	static Enum getDefault() {
		return NoSegmentation;
	}
};
typedef EnumTemplate<SegmentationEnumDefinition> SegmentationEnum;


struct ServicesSupportedEnumDefinition {
	enum Enum {
	// Alarm and Event Services
		AcknowledgeAlarm = 0,
		ConfirmedCOVNotification = 1,
		ConfirmedEventNotification = 2,
		GetAlarmSummary = 3,
		GetEnrollmentSummary = 4,
		GetEventInformation = 39,
		SubscribeCOV = 5,
		SubscribeCOVProperty = 38,
		LifeSafetyOperation = 37,

	// File Access Services
		AtomicReadFile = 6,
		AtomicWriteFile = 7,

	// Object Access Services
		AddListElement = 8,
		RemoveListElement = 9,
		CreateObject = 10,
		DeleteObject = 11,
		ReadProperty = 12,
		ReadPropertyConditional = 13,
		ReadPropertyMultiple = 14,
		ReadRange = 35,
		WriteProperty = 15,
		WritePropertyMultiple = 16,

	// Remote Device Management Services
		DeviceCommunicationControl = 17,
		ConfirmedPrivateTransfer = 18,
		ConfirmedTextMessage = 19,
		ReinitializeDevice = 20,
		UtcTimeSynchronization = 36,

	// Virtual Terminal Services
		VtOpen = 21,
		VtClose = 22,
		VtData = 23,

	// Security Services
		Authenticate = 24,
		RequestKey = 25,

	// Unconfirmed Services
		IAm = 26,
		IHave = 27,
		UnconfirmedCOVNotification = 28,
		UnconfirmedEventNotification = 29,
		UnconfirmedPrivateTransfer = 30,
		UnconfirmedTextMessage = 31,
		TimeSynchronization = 32,
		WhoHas = 33,
		WhoIs = 34
	};

	static const char* getTypeName() {
		return "Supported Services";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case AcknowledgeAlarm:
			return "Acknowledge Alarm";
		case ConfirmedCOVNotification:
			return "Confirmed COV Notification";
		case ConfirmedEventNotification:
			return "Confirmed Event Notification";
		case GetAlarmSummary:
			return "Get Alarm Summary";
		case GetEnrollmentSummary:
			return "Get Enrollment Summary";
		case GetEventInformation:
			return "Get Event Information";
		case SubscribeCOV:
			return "Subscribe COV";
		case SubscribeCOVProperty:
			return "Subscribe COV Property";
		case LifeSafetyOperation:
			return "Life Safety Operation";
		case AtomicReadFile:
			return "Atomic Read File";
		case AtomicWriteFile:
			return "Atomic Write File";
		case AddListElement:
			return "Add List Element";
		case RemoveListElement:
			return "Remove List Element";
		case CreateObject:
			return "Create Object";
		case DeleteObject:
			return "Delete Object";
		case ReadProperty:
			return "Read Property";
		case ReadPropertyConditional:
			return "Read Property Conditional";
		case ReadPropertyMultiple:
			return "Read Property Multiple";
		case ReadRange:
			return "Read Range";
		case WriteProperty:
			return "Write Property";
		case WritePropertyMultiple:
			return "Write Property Multiple";
		case DeviceCommunicationControl:
			return "Device Communication Control";
		case ConfirmedPrivateTransfer:
			return "Confirmed Private Transfer";
		case ConfirmedTextMessage:
			return "Confirmed Text Message";
		case ReinitializeDevice:
			return "Reinitialize Device";
		case UtcTimeSynchronization:
			return "UTC Time Synchronization";
		case VtOpen:
			return "VT Open";
		case VtClose:
			return "VT Close";
		case VtData:
			return "VT Data";
		case Authenticate:
			return "Authenticate";
		case RequestKey:
			return "Request Key";
		case IAm:
			return "I Am";
		case IHave:
			return "I Have";
		case UnconfirmedCOVNotification:
			return "Unconfirmed COV Notification";
		case UnconfirmedEventNotification:
			return "Unconfirmed Event Notification";
		case UnconfirmedPrivateTransfer:
			return "Unconfirmed Private Transfer";
		case UnconfirmedTextMessage:
			return "Unconfirmed Text Message";
		case TimeSynchronization:
			return "Time Synchronization";
		case WhoHas:
			return "Who Has";
		case WhoIs:
			return "Who Is";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return ReadProperty;
	}
};
typedef EnumTemplate<ServicesSupportedEnumDefinition> ServicesSupportedEnum;


class AbortReasonEnumDefinition {
public:
	enum Enum {
		Other,
		BufferOverflow,
		InvalidApduInThisState,
		PreemptedByHigherPriorityTask,
		SegmentationNotSupported
	};

	static const char* getTypeName() {
		return "Abort Reason";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Other:
			return "Other";
		case BufferOverflow:
			return "Buffer Overflow";
		case InvalidApduInThisState:
			return "Invalid APDU In This State";
		case PreemptedByHigherPriorityTask:
			return "Preempted By Higher Priority Task";
		case SegmentationNotSupported:
			return "Segmentation Not Supported";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Other;
	}
};
typedef EnumTemplate<AbortReasonEnumDefinition> AbortReasonEnum;


class RejectReasonEnumDefinition {
public:
	enum Enum {
		Other,
		BufferOverflow,
		InconsistentParameters,
		InvalidParameterDataType,
		InvalidTag,
		MissingRequiredParameter,
		ParameterOutOfRange,
		TooManyArguments,
		UndefinedEnumeration,
		UnrecognizedService
	};

	static const char* getTypeName() {
		return "Reject Reason";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Other:
			return "Other";
		case BufferOverflow:
			return "Buffer Overflow";
		case InconsistentParameters:
			return "Inconsistent Parameters";
		case InvalidParameterDataType:
			return "Invalid Parameter Data Type";
		case InvalidTag:
			return "Invalid Tag";
		case MissingRequiredParameter:
			return "Missing Required Parameter";
		case ParameterOutOfRange:
			return "Parameter Out Of Range";
		case TooManyArguments:
			return "Too Many Arguments";
		case UndefinedEnumeration:
			return "Undefined Enumeration";
		case UnrecognizedService:
			return "Unrecognized Service";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Other;
	}
};
typedef EnumTemplate<RejectReasonEnumDefinition> RejectReasonEnum;


class ErrorClassEnumDefinition {
public:
	enum Enum {
		Device,
		Object,
		Property,
		Resources,
		Security,
		Services,
		Vt,
		Communication
	};

	static const char* getTypeName() {
		return "Error Class";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Device:
			return "Device";
		case Object:
			return "Object";
		case Property:
			return "Property";
		case Resources:
			return "Resources";
		case Security:
			return "Security";
		case Services:
			return "Services";
		case Vt:
			return "VT";
		case Communication:
			return "Communication";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Property;
	}
};
typedef EnumTemplate<ErrorClassEnumDefinition> ErrorClassEnum;


class ErrorCodeEnumDefinition {
public:
	enum Enum {
		Other,
		AbortBufferOverflow = 51,
		AbortInvalidApduInThisState = 52,
		AbortPreemptedByHigherPriority = 53,
		AbortSegmentationNotSupported = 54,
		AbortProprietary = 55,
		AbortOther = 56,
		AuthenticationFailed = 1,
		CharacterSetNotSupported = 41,
		CommunicationDisabled = 83,
		ConfigurationInProgress = 2,
		DatatypeNotSupported = 47,
		DeviceBusy = 3,
		DuplicateName = 48,
		DuplicateObjectId = 49,
		DynamicCreationNotSupported = 4,
		FileAccessDenied = 5,
		IncompatibleSecurityLevels = 6,
		InconsistentParameters = 7,
		InconsistentSelectionCriterion = 8,
		InvalidArrayIndex = 42,
		InvalidConfigurationData = 46,
		InvalidDataType = 9,
		InvalidEventState = 73,
		InvalidFileAccessMethod = 10,
		InvalidFileStartPosition = 11,
		InvalidOperatorName = 12,
		InvalidParameterDataType = 13,
		InvalidTag = 57,
		InvalidTimeStamp = 14,
		KeyGenerationError = 15,
		LogBufferFull = 75,
		LoggedValuePurged = 76,
		MissingRequiredParameter = 16,
		NetworkDown = 58,
		NoAlarmConfigured = 74,
		NoObjectsOfSpecifiedType = 17,
		NoPropertySpecified = 77,
		NoSpaceForObject = 18,
		NoSpaceToAddListElement = 19,
		NoSpaceToWriteProperty = 20,
		NoVtSessionAvailable = 21,
		NotConfiguredForTriggeredLogging = 78,
		ObjectDeletionNotPermitted = 23,
		ObjectIdentifierAlreadyExists = 24,
		OperationalProblem = 25,
		OptionalFunctionalityNotSupported = 45,
		PasswordFailure = 26,
		PropertyIsNotAList = 22,
		PropertyIsNotAnArray = 50,
		ReadAccessDenied = 27,
		RejectBufferOverflow = 59,
		RejectInconsistentParameters = 60,
		RejectInvalidParameterDataType = 61,
		RejectInvalidTag = 62,
		RejectMissingRequiredParameter = 63,
		RejectParameterOutOfRange = 64,
		RejectTooManyArguments = 65,
		RejectUndefinedEnumeration = 66,
		RejectUnrecognizedService = 67,
		RejectProprietary = 68,
		RejectOther = 69,
		SecurityNotSupported = 28,
		ServiceRequestDenied = 29,
		Timeout = 30,
		UnknownDevice = 70,
		UnknownObject = 31,
		UnknownProperty = 32,
		UnknownVtClass = 34,
		UnknownVtSession = 35,
		UnsupportedObjectType = 36,
		ValueOutOfRange = 37,
		VtSessionAlreadyClosed = 38,
		VtSessionTerminationFailure = 39,
		WriteAccessDenied = 40,
		CovSubscriptionFailed = 43,
		NotCovProperty = 44
	};

	static const char* getTypeName() {
		return "Error Class";
	}

	static const char* getName(Enum e) {
		switch (e) {
		case Other:
			return "Other";
		case AbortBufferOverflow:
			return "Abort Buffer Overflow";
		case AbortInvalidApduInThisState:
			return "Abort Invalid APDU In This State";
		case AbortPreemptedByHigherPriority:
			return "Abort Preempted By Higher Priority";
		case AbortSegmentationNotSupported:
			return "Abort Segmentation Not Supported";
		case AbortProprietary:
			return "Abort Proprietary";
		case AbortOther:
			return "Abort Other";
		case AuthenticationFailed:
			return "Authentication Failed";
		case CharacterSetNotSupported:
			return "Character Set Not Supported";
		case CommunicationDisabled:
			return "Communication Disabled";
		case ConfigurationInProgress:
			return "Configuration In Progress";
		case DatatypeNotSupported:
			return "Data Type Not Supported";
		case DeviceBusy:
			return "Device Busy";
		case DuplicateName:
			return "Duplicate Name";
		case DuplicateObjectId:
			return "Duplicate Object Id";
		case DynamicCreationNotSupported:
			return "Dynamic Creation Not Supported";
		case FileAccessDenied:
			return "File Access Denied";
		case IncompatibleSecurityLevels:
			return "Incompatible Security Levels";
		case InconsistentParameters:
			return "Inconsistent Parameters";
		case InconsistentSelectionCriterion:
			return "Inconsistent Selection Criterion";
		case InvalidArrayIndex:
			return "Invalid Array Index";
		case InvalidConfigurationData:
			return "Invalid Configuration Data";
		case InvalidDataType:
			return "Invalid Data Type";
		case InvalidEventState:
			return "Invalid Event State";
		case InvalidFileAccessMethod:
			return "Invalid File Access Method";
		case InvalidFileStartPosition:
			return "Invalid File Start Position";
		case InvalidOperatorName:
			return "Invalid Operator Name";
		case InvalidParameterDataType:
			return "Invalid Parameter Data Type";
		case InvalidTag:
			return "Invalid Tag";
		case InvalidTimeStamp:
			return "Invalid Time Stamp";
		case KeyGenerationError:
			return "Key Generation Error";
		case LogBufferFull:
			return "Log Buffer Full";
		case LoggedValuePurged:
			return "Logged Value Purged";
		case MissingRequiredParameter:
			return "Missing Required Parameter";
		case NetworkDown:
			return "Network Down";
		case NoAlarmConfigured:
			return "No Alarm Configured";
		case NoObjectsOfSpecifiedType:
			return "No Objects Of Specified Type";
		case NoPropertySpecified:
			return "No Property Specified";
		case NoSpaceForObject:
			return "No Space For Object";
		case NoSpaceToAddListElement:
			return "No Space To Add List Element";
		case NoSpaceToWriteProperty:
			return "No Space To Write Property";
		case NoVtSessionAvailable:
			return "No VT Session Available";
		case NotConfiguredForTriggeredLogging:
			return "Not Configured For Triggered Logging";
		case ObjectDeletionNotPermitted:
			return "Object Deletion Not Permitted";
		case ObjectIdentifierAlreadyExists:
			return "Object Identifier Already Exists";
		case OperationalProblem:
			return "Operational Problem";
		case OptionalFunctionalityNotSupported:
			return "Optional Functionality Not Supported";
		case PasswordFailure:
			return "Password Failure";
		case PropertyIsNotAList:
			return "Property Is Not A List";
		case PropertyIsNotAnArray:
			return "Property Is Not An Array";
		case ReadAccessDenied:
			return "Read Access Denied";
		case RejectBufferOverflow:
			return "Reject Buffer Overflow";
		case RejectInconsistentParameters:
			return "Reject Inconsistent Parameters";
		case RejectInvalidParameterDataType:
			return "Reject Invalid Parameter Data Type";
		case RejectInvalidTag:
			return "Reject Invalid Tag";
		case RejectMissingRequiredParameter:
			return "Reject Missing Required Parameter";
		case RejectParameterOutOfRange:
			return "Reject Parameter Out Of Range";
		case RejectTooManyArguments:
			return "Reject Too Many Arguments";
		case RejectUndefinedEnumeration:
			return "Reject Undefined Enumeration";
		case RejectUnrecognizedService:
			return "Reject Unrecognized Service";
		case RejectProprietary:
			return "Reject Proprietary";
		case RejectOther:
			return "Reject Other";
		case SecurityNotSupported:
			return "Security Not Supported";
		case ServiceRequestDenied:
			return "Service Request Denied";
		case Timeout:
			return "Timeout";
		case UnknownDevice:
			return "Unknown Device";
		case UnknownObject:
			return "Unknown Object";
		case UnknownProperty:
			return "Unknown Property";
		case UnknownVtClass:
			return "Unknown VT Class";
		case UnknownVtSession:
			return "Unknown VT Session";
		case UnsupportedObjectType:
			return "Unsupported Object Type";
		case ValueOutOfRange:
			return "Value Out Of Range";
		case VtSessionAlreadyClosed:
			return "VT Session Already Closed";
		case VtSessionTerminationFailure:
			return "VT Session Termination Failure";
		case WriteAccessDenied:
			return "Write Access Denied";
		case CovSubscriptionFailed:
			return "COV Subscription Failed";
		case NotCovProperty:
			return "Not COV Property";
		default:
			return "Unknown";
		}
	}

	static Enum getDefault() {
		return Other;
	}
};
typedef EnumTemplate<ErrorCodeEnumDefinition> ErrorCodeEnum;


} //BACNET namepsace

#endif /* BACNETENUMS_H_ */

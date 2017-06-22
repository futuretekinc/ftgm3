#include "defined.h"
#include "utils.h"
#include "device_gtc_520a.h"


DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager)
: DeviceModbus(_manager, NODE_TYPE_DEV_GTC_520A, true)
{
	serial_->SetBaudrate(9600);
	serial_->SetParityBit("even");
}

DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, NODE_TYPE_DEV_GTC_520A)
{
	TRACE_INFO("GTE520A Created");

	serial_->SetBaudrate(9600);
	serial_->SetParityBit("even");

	SetProperties(_properties, false, true);
}

bool	DeviceGTC520A::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
	TRACE_INFO("ReadValue(" << _id << ")");
	if (_id == "30001")
	{
		int16_t	value;

		if (!ReadHoldingRegisters(1, &value, 1))
		{
			TRACE_ERROR("Failed to read register");
			return	false;	
		}

		_time = time_t(Date::GetCurrent());
		_value = ToString(value);
	}
	else
	{
		return	false;	
	}

	return	true;
}

void	DeviceGTC520A::Process()
{
	DeviceModbus::Process();
}

const	std::string&	DeviceGTC520A::Type()
{
	static	std::string	type_(NODE_TYPE_DEV_GTC_520A);

	return	type_;
}


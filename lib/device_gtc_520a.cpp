#include "defined.h"
#include "utils.h"
#include "device_gtc_520a.h"


DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager)
: DeviceModbus(_manager, NODE_TYPE_DEV_GTC_520A, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);
}

DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, NODE_TYPE_DEV_GTC_520A, true), correction_interval_(1)
{
	TRACE_INFO("GTE520A Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

bool	DeviceGTC520A::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
	TRACE_INFO("ReadValue(" << _id << ")");
	if (_id == "30001")
	{
		_time = time_;
		_value = ToString(registers_[0]);
	}
	else
	{
		return	false;	
	}

	return	true;
}

void	DeviceGTC520A::Preprocess()
{
	DeviceModbus::Preprocess();
}

void	DeviceGTC520A::Process()
{
	if(correction_timer_.RemainTime() == 0)
	{
		if (!ReadHoldingRegisters(1, registers_, 2))
		{
			TRACE_ERROR("Failed to read register");
		}
		else
		{
			time_ = Date::GetCurrent();
		}

		correction_timer_ += correction_interval_ * TIME_SECOND;
	}

	DeviceModbus::Process();
}

const	std::string&	DeviceGTC520A::Type()
{
	static	std::string	type_(NODE_TYPE_DEV_GTC_520A);

	return	type_;
}


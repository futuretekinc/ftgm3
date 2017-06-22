#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
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
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_id);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _id << "] is not attached");
			return	false;	
		}
		if (endpoint->GetSensorID() == "30001")
		{
			_time = time_;
			_value = ToString(registers_[0]);
		}
		else if (endpoint->GetSensorID() == "30002")
		{
			_time = time_;
			_value = ToString(registers_[1]);
		}
		else
		{
			return	false;	
		}

		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	DeviceGTC520A::Preprocess()
{
	DeviceModbus::Preprocess();
}

void	DeviceGTC520A::Process()
{
	if(correction_timer_.RemainTime() == 0)
	{
		if (!ReadHoldingRegisters(0, registers_, 2))
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


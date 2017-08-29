#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_gtc_520a.h"


static const char*	class_name = "DeviceGTC520A";

DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_GTC_520A, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);
}

DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_GTC_520A, true), correction_interval_(1)
{
	TRACE_INFO("GTE520A Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceGTC520A::GetClassName()
{
	return	class_name;
}

bool	DeviceGTC520A::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_epid);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] is not attached");
			return	false;	
		}

		uint32_t	type 	= endpoint->GetSensorID() / 10000;
		uint32_t	index 	= endpoint->GetSensorID() % 10000 - 1; 

		if ((type != 3) || (index > 1))
		{
			TRACE_ERROR("Invalid sensro id!");
			return	false;
		}

		_time = time_;
		_value = ToString(registers_[index]);

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
		TRACE_INFO("GTC520A read register");
		if (!ReadHoldingRegisters(0, registers_, 2))
		{
			TRACE_ERROR("Failed to read register");
		}
		else
		{
			time_ = Date::GetCurrent();
		}

		correction_timer_.Add(correction_interval_);
		TRACE_INFO("GTC520A Remain Time : " << correction_timer_.RemainTime());
	}

	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceGTC520A::Type()
{
	return	OBJECT_TYPE_DEV_GTC_520A;
}


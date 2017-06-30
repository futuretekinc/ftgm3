#include <iomanip>
#include "utils.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_adam_6051.h"


DeviceADAM6051::DeviceADAM6051(ObjectManager& _manager)
: DeviceModbusTCP(_manager, NODE_TYPE_DEV_ADAM_6051), correction_interval_(1)
{
}

DeviceADAM6051::DeviceADAM6051(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbusTCP(_manager, NODE_TYPE_DEV_ADAM_6051), correction_interval_(1)
{
	TRACE_INFO("Advantech ADAM6051 Created");


	SetProperties(_properties, false, true);
}

bool	DeviceADAM6051::ReadValue(std::string const& _id, time_t& _time, uint32_t& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_id);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _id << "] is not attached");
			return	false;	
		}

		uint32_t	index = strtoul(endpoint->GetSensorID().c_str(), 0, 10);
		if (index < 28)
		{
			int16_t	values[2];

			if (!ReadInputRegisters(index, values, 2))
			{
				TRACE_ERROR("Failed to read input registers!");
				return	false;	
			}
			_value = ((uint32_t)values[0] << 16) + values[1];
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

bool	DeviceADAM6051::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_id);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _id << "] is not attached");
			return	false;	
		}

		uint32_t	index = strtoul(endpoint->GetSensorID().c_str(), 0, 10);
		if (index < 28)
		{
			int16_t	values[2];

			if (!ReadInputRegisters(index, values, 2))
			{
				TRACE_ERROR("Failed to read input registers!");
				return	false;	
			}
			uint32_t	value = ((uint32_t)values[0] << 16) + values[1];

			_value = ToString(value);

			TRACE_INFO("Value : " << value);
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	DeviceADAM6051::Preprocess()
{
	DeviceModbusTCP::Preprocess();
}

void	DeviceADAM6051::Process()
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

	DeviceModbusTCP::Process();
}

const	std::string&	DeviceADAM6051::Type()
{
	static	std::string	type_(NODE_TYPE_DEV_ADAM_6051);

	return	type_;
}


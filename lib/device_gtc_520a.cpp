#include "defined.h"
#include "utils.h"
#include "device_gtc_520a.h"


DeviceGTC520A::DeviceGTC520A(ObjectManager& _manager)
: DeviceModbus(_manager, NODE_TYPE_DEV_GTC_520A)
{
	baudrate_ = B9600;
	parity_bit_ = PARENB;
}

bool	DeviceGTC520A::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
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


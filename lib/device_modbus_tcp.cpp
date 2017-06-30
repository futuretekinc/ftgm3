#include <errno.h>
#include "device_modbus_tcp.h"


DeviceModbusTCP::DeviceModbusTCP(ObjectManager& _manager, std::string const& _type)
: DeviceIP(_manager, _type), modbus_(NULL)
{
	port_ = 502;
}

void	DeviceModbusTCP::Preprocess()
{
}

bool	DeviceModbusTCP::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	return	false;
}

bool	DeviceModbusTCP::ReadValue(std::string const& _epid, time_t& _time, uint32_t& _value)
{
	return	false;
}

bool	DeviceModbusTCP::ReadValue(std::string const& _epid, time_t& _time, bool& _value)
{
	return	false;
}

bool	DeviceModbusTCP::ReadHoldingRegisters(uint16_t address, int16_t* values, uint16_t count)
{
	if (!IsConnected())
	{
		if (!Connect())
		{
			TRACE_ERROR("Failed to connect to " << ip_);
			return	false;	
		}
	}

	return	true;
}

bool	DeviceModbusTCP::ReadInputRegisters(uint16_t address, int16_t* values, uint16_t count)
{
	if (!IsConnected())
	{
		if (!Connect())
		{
			TRACE_ERROR("Failed to connect to " << ip_);
			return	false;	
		}
	}

	if (modbus_read_input_registers(modbus_, address, count, (uint16_t *)values) != count)
	{
		TRACE_ERROR("Failed to read input register[" << errno << "]");
		return	false;
	}

	return	true;
}

bool	DeviceModbusTCP::WriteRegister(uint16_t address, uint16_t value)
{
	if (!IsConnected())
	{
		if (!Connect())
		{
			TRACE_ERROR("Failed to connect to " << ip_);
			return	false;	
		}
	}

	return	true;
}

bool	DeviceModbusTCP::RequestAndWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout)
{
}

bool	DeviceModbusTCP::IsConnected()
{
	return	(modbus_ != NULL);
}

bool	DeviceModbusTCP::Connect()
{
	if (modbus_ == NULL)
	{
		modbus_t*	modbus;

		modbus = modbus_new_tcp(ip_.c_str(), port_);

		if (modbus_connect(modbus) == 0)
		{
			modbus_ = modbus;

			return	true;
		}

		modbus_free(modbus);

		TRACE_ERROR("MODBUS connectino error[" << ip_.c_str() << ":" << port_ << "]");
		return	false;
	}

	return	true;
}

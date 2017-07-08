#include <errno.h>
#include "device_modbus_tcp.h"


DeviceModbusTCP::DeviceModbusTCP(ObjectManager& _manager, std::string const& _type)
: DeviceIP(_manager, _type), modbus_(NULL), request_fail_count_(0), request_retry_count_(5)
{
	port_ = 502;
}

void	DeviceModbusTCP::Preprocess()
{
	Device::Preprocess();
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

	if (modbus_read_input_registers(modbus_, address, count, (uint16_t *)values) != count)
	{
		request_fail_count_++;
		TRACE_ERROR("Failed to read holding register[" << request_fail_count_ << " - " << modbus_strerror(errno) << "]");

		if (request_retry_count_ <= request_fail_count_)
		{
			Disconnect();	
		}

		return	false;
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
		request_fail_count_++;
		TRACE_ERROR("Failed to read input register[" << request_fail_count_ << " - " << modbus_strerror(errno) << "]");

		if (request_retry_count_ <= request_fail_count_)
		{
			Disconnect();	
		}

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
			request_fail_count_ = 0;
			
			TRACE_INFO("Connected to the device[" << ip_ << ":" << port_ << "] using modbus/tcp");
			return	true;
		}

		modbus_free(modbus);

		TRACE_ERROR("Failed to connect with MODBUS/TCP![" << ip_ << ":" << port_ << "]");
		return	false;
	}

	return	true;
}

bool	DeviceModbusTCP::Disconnect()
{
	if (modbus_ != NULL)
	{
		modbus_close(modbus_);
		modbus_free(modbus_);

		modbus_ = NULL;

		TRACE_ERROR("MODBUS disconnected!");
	}

	return	true;
}

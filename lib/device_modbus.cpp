#include <arpa/inet.h>
#include <exception>
#include "timer.h"
#include "device_modbus.h"
#include "modbus.h"

using namespace modbus;

DeviceModbus::DeviceModbus(ObjectManager& _manager, std::string const& _type, bool _half)
: DeviceSerial(_manager, _type, _half), timeout_(100000)
{
}

bool	DeviceModbus::ReadValue(std::string const& _id, time_t& _time, std::string& _value)
{
	return	false;
}

bool	DeviceModbus::ReadValue(std::string const& _id, time_t& _time, bool& _value)
{
	return	false;
}

bool	DeviceModbus::RequestAndWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout)
{
	Timer		response_timeout;

	if (!serial_.Write(_request, _request_len))
	{
		TRACE_ERROR("Failed to send frame!");
		return	false;
	}

	response_timeout.Add(timeout_);
	_response_len = 0;
	while(response_timeout.RemainTime() > 0)
	{
		uint32_t	read_len = 0;	

		if (serial_.Read(&_response[_response_len], _max_response_len - _response_len, 20, read_len))
		{
			_response_len += read_len;

			if (_response_len >= _max_response_len)
			{
				break;
			}
		}
	}

	TRACE_INFO("Read");
	TRACE_INFO_DUMP((char *)_response, _response_len);


	if (_response_len < _max_response_len)
	{
		TRACE_ERROR("Failed to receive frame!");
		return	false;	
	}

	return	true;
}

bool	DeviceModbus::ReadHoldingRegisters(uint16_t address, int16_t *values, uint16_t count)
{
	RequestFrame	request(1, READ_HOLDING_REGISTERS, address, count);
	uint8_t			buffer[MODBUS_FRAME_SIZE];
	uint32_t		response_len = 0;

	if (!RequestAndWait(request.Raw(), request.Size(), buffer, request.DesiredResponseSize(), response_len, timeout_))
	{
		TRACE_ERROR("Failed to read holding registers!");
		return	false;
	}

	try
	{
		ResponseFrame	response(1, READ_HOLDING_REGISTERS, buffer, response_len);

		if (!response.IsValid())
		{
			TRACE_ERROR("Failed to read holding registers!");
			return	false;
		}

		if (!response.ReadHoldingRegisters(values, count))
		{
			TRACE_ERROR("Failed to read holding registers!");
			return	false;
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Invalid response : " << e.what());
		return	false;
	}

	return	true;
}

bool	DeviceModbus::WriteRegister(uint16_t address, uint16_t value)
{
	RequestFrame	request(1, WRITE_SINGLE_REGISTER, address, value);
	uint8_t			buffer[MODBUS_FRAME_SIZE];
	uint32_t		response_len = 0;

	if (!RequestAndWait(request.Raw(), request.Size(), buffer, request.DesiredResponseSize(), response_len, timeout_))
	{
		TRACE_ERROR("Failed to write single register!");
		return	false;
	}

	try
	{
		ResponseFrame	response(1, WRITE_SINGLE_REGISTER, buffer, response_len);

		if (!response.IsValid())
		{
			TRACE_ERROR("Failed to write single register!");
			return	false;
		}

		if (!response.WriteSingleRegister())
		{
			TRACE_ERROR("Failed to write single register!");
			return	false;
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Invalid response : " << e.what());
		return	false;
	}

	return	true;
}

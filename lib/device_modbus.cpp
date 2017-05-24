#include <arpa/inet.h>
#include <exception>
#include "timer.h"
#include "device_modbus.h"
#include "modbus.h"

using namespace modbus;

DeviceModbus::DeviceModbus(ObjectManager& _manager, ValueType const& _type)
: DeviceSerial(_manager, _type), timeout_(1000)
{
}

bool	DeviceModbus::RequestAndWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout)
{
	Timer		response_timeout;

	if (!Write(_request, _request_len))
	{
		TRACE_ERROR("Failed to send frame!");
		return	false;
	}

	response_timeout += timeout_;
	_response_len = 0;
	while(response_timeout.RemainTime() > 0)
	{
		uint32_t	read_len = 0;	

		if (Read(&_response[_response_len], _max_response_len - _response_len, read_len))
		{
			_response_len += read_len;

			if (_response_len >= _max_response_len)
			{
				break;
			}
		}
	}


	if (_response_len < _max_response_len)
	{
		TRACE_ERROR("Failed to receive frame!");
		return	false;	
	}

	return	true;
}

bool	DeviceModbus::ReadHoldingRegisters(uint16_t address, int16_t *values, uint16_t count)
{
	RequestFrame	request(READ_HOLDING_REGISTERS, address, count);
	uint8_t			buffer[MODBUS_FRAME_SIZE];
	uint32_t		response_len = 0;

	if (!RequestAndWait(request.Raw(), request.Size(), buffer, request.DesiredResponseSize(), response_len, timeout_))
	{
		TRACE_ERROR("Failed to read holding registers!");
		return	false;
	}

	try
	{
		ResponseFrame	response(READ_HOLDING_REGISTERS, buffer, response_len);

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
	RequestFrame	request(WRITE_SINGLE_REGISTER, address, value);
	uint8_t			buffer[MODBUS_FRAME_SIZE];
	uint32_t		response_len = 0;

	if (!RequestAndWait(request.Raw(), request.Size(), buffer, request.DesiredResponseSize(), response_len, timeout_))
	{
		TRACE_ERROR("Failed to write single register!");
		return	false;
	}

	try
	{
		ResponseFrame	response(WRITE_SINGLE_REGISTER, buffer, response_len);

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

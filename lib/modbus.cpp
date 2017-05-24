#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>
#include "modbus.h"
#include "trace.h"

using namespace modbus;

RequestFrame::RequestFrame(FunctionCode _function, uint16_t _address, uint16_t _value)
{
	switch(_function)
	{
	case	READ_HOLDING_REGISTERS:
		{
			frame_.read_holding_registers.function	= _function;
			frame_.read_holding_registers.address	= htons(_address);
			frame_.read_holding_registers.quantity	= htons(_value);
			frame_size_ = 5;
			response_size_ = 2 + _value*2;
		}
		break;

	case	WRITE_SINGLE_REGISTER:
		{
			frame_.write_single_register.function	= _function;
			frame_.write_single_register.address	= htons(_address);
			frame_.write_single_register.value		= htons(_value);
			frame_size_ = 5;
			response_size_ = 5;
		}
		break;
	}
}

uint32_t	RequestFrame::Size()
{
	return	frame_size_;
}

uint32_t	RequestFrame::DesiredResponseSize()
{
	return	response_size_;
}

uint8_t*	RequestFrame::Raw()
{
	return	frame_.raw;
}

ResponseFrame::ResponseFrame(FunctionCode _function, uint8_t* _buffer, uint32_t _length)
: valid_(false)
{
	if (_length < 2 || _length > MODBUS_FRAME_SIZE)
	{
		TRACE_ERROR("Frame is too long.");
	}
	else if (_buffer[0] != _function)
	{
		TRACE_ERROR("Function code mismatch.");
	}
	else
	{
		frame_size_ = _length;
		memcpy(frame_.raw, _buffer, _length);

		switch(frame_.common.function)
		{
		case	READ_HOLDING_REGISTERS:
			{
				if (frame_size_ == (2 + frame_.read_holding_registers.count*2))
				{
					valid_ = true;	
				}
			}			
			break;

		case	WRITE_SINGLE_REGISTER:
			{
				if (frame_size_ == 5)
				{
					valid_ = true;	
				}
			}			
			break;
		}
	}
}

bool	ResponseFrame::ReadHoldingRegisters(int16_t* values, uint32_t _count)
{
	if (frame_.read_holding_registers.function != READ_HOLDING_REGISTERS)
	{
		TRACE_ERROR("Invalid response function code!");
		return	false;	
	}
	
	if (frame_.read_holding_registers.count != _count*2)	
	{
		TRACE_ERROR("Invalid response byte count!");
		return	false;
	}

	for(int i = 0 ; i < _count ; i++)
	{
		values[i] = ntohs(frame_.read_holding_registers.values[i]);
	}

	return	true;
}

bool	ResponseFrame::IsValid()
{
	return	valid_;
}

uint8_t	ResponseFrame::ErrorCode()
{
	return	frame_.error.code;
}

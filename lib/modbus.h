#ifndef	MODBUS_H_

#include <stdint.h>

namespace	modbus
{
#define	MODBUS_FRAME_SIZE	256

#pragma pack(push, 1)

enum	FunctionCode
{
	READ_HOLDING_REGISTERS 	= 0x03,
	WRITE_SINGLE_REGISTER	= 0x06
};

struct	RequestReadHoldingRegisters
{
	uint8_t		function;
	uint16_t	address;
	uint16_t	quantity;
};

struct	RequestWriteSingleRegister
{
	uint8_t		function;
	uint16_t	address;
	uint16_t	value;
};

class	RequestFrame
{
public:
	RequestFrame(FunctionCode _function, uint16_t _address, uint16_t _value);

	uint32_t	Size();
	uint32_t	DesiredResponseSize();
	uint8_t*	Raw();
private:
	union	
	{
		RequestReadHoldingRegisters	read_holding_registers;
		RequestWriteSingleRegister	write_single_register;
		uint8_t						raw[MODBUS_FRAME_SIZE];
	}	frame_;

	uint32_t	frame_size_;
	uint32_t	response_size_;
};

struct	ResponseCommon
{
	uint8_t		function;
};

struct	ResponseReadHoldingRegisters
{
	uint8_t		function;
	uint8_t		count;
	int16_t		values[];
};

struct	ResponseWriteSingleRegister
{
	uint8_t		function;
	uint16_t	address;
	uint16_t	value;
};

struct	ResponseError
{
	uint8_t		code;
	uint8_t		exception;
};


class	ResponseFrame
{
public:
	ResponseFrame(FunctionCode _function, uint8_t* _buffer, uint32_t _length);

	bool	IsValid();
	bool	ReadHoldingRegisters(int16_t* values, uint32_t _count);
	bool	WriteSingleRegister();
	uint8_t	ErrorCode();

private:
	bool		valid_;
	uint32_t	frame_size_;
	union
	{
		ResponseCommon					common;
		ResponseReadHoldingRegisters	read_holding_registers;
		ResponseWriteSingleRegister		write_single_register;
		ResponseError					error;
		uint8_t							raw[MODBUS_FRAME_SIZE];
	}	frame_;
};

#pragma pack(pop)
};

#endif

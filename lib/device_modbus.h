#ifndef	DEVICE_MODBUS_H_
#define	DEVICE_MODBUS_H_

#include "device_serial.h"


class	DeviceModbus : public DeviceSerial
{
public:
	DeviceModbus(ObjectManager& _manager, std::string const& _type);

	bool	ReadHoldingRegisters(uint16_t address, int16_t* values, uint16_t count);
	bool	WriteRegister(uint16_t address, uint16_t value);

protected:
	bool	RequestAndWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout);

	uint32_t	timeout_;

};
#endif

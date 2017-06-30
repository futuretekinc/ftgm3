#ifndef	DEVICE_MODBUS_TCP_H_
#define	DEVICE_MODBUS_TCP_H_

#include "device_ip.h"
#include "modbus/modbus-tcp.h"

class	DeviceModbusTCP : public DeviceIP
{
public:
	DeviceModbusTCP(ObjectManager& _manager, std::string const& _type);

	virtual	bool	ReadValue(std::string const& _epid, time_t& _time, std::string& _value);
	virtual	bool	ReadValue(std::string const& _epid, time_t& _time, uint32_t& _value);
	virtual	bool	ReadValue(std::string const& _epid, time_t& _time, bool& _value);

protected:
			void	Preprocess();

			bool	IsConnected();
			bool	Connect();

			bool	ReadHoldingRegisters(uint16_t address, int16_t* values, uint16_t count);
			bool	ReadInputRegisters(uint16_t address, int16_t* values, uint16_t count);
			bool	WriteRegister(uint16_t address, uint16_t value);
			bool	RequestAndWait(uint8_t* _request, uint32_t _request_len, uint8_t* _response, uint32_t _max_response_len, uint32_t& _response_len, uint32_t _timeout);

	uint32_t	timeout_;
	modbus_t*	modbus_;
};
#endif

#ifndef	DEVICE_SERIAL_H_
#define	DEVICE_SERIAL_H_

#include "device.h"

class	DeviceSerial : public Device
{
public:
	DeviceSerial(Type _type);
	DeviceSerial(Type _type, std::string const& _port);

	virtual	bool	GetProperties(Properties& _properties) const;
	virtual	bool	SetProperty(Property const& _property);

	const 	std::string&	GetPort();
			bool		SetPort(const std::string& _port);
protected:
	std::string	port_;
};

#endif

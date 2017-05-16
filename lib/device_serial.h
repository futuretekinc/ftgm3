#ifndef	DEVICE_SERIAL_H_
#define	DEVICE_SERIAL_H_

#include "device.h"

class	DeviceSerial : public Device
{
public:
	DeviceSerial(ObjectManager& _manager, Type _type);
	DeviceSerial(ObjectManager& _manager, Type _type, std::string const& _port);

	virtual	bool	GetProperties(Properties& _properties) const;

	const 	std::string&	GetPort();
			bool		SetPort(const std::string& _port);
protected:
	virtual	bool	SetPropertyInternal(Property const& _property, bool create = false);

	std::string	port_;
};

#endif

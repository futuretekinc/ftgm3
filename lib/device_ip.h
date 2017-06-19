#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(ObjectManager& _manager, std::string const& _type);
	DeviceIP(ObjectManager& _manager, std::string const& _type, const std::string& _ip);

			bool		IsIncludedIn(std::string const& _type);

	virtual	bool		GetProperties(JSONNode& _properties, Fields const& _fields = PROPERTY_ALL);

	virtual	bool		SetProperty(JSONNode const& _property, bool _check = false);

	const std::string&	GetIP();
			bool		SetIP(const std::string& _ip, bool _check = false);

	static	const	std::string&	Type();

protected:

	std::string	ip_;
};

#endif

#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(ObjectManager& _manager, std::string const& _type);
	DeviceIP(ObjectManager& _manager, std::string const& _type, const std::string& _ip);

			bool		IsIncludedIn(std::string const& _type);

	virtual	bool		GetOptions(JSONNode& _properties);
	virtual	bool		SetOption(JSONNode const& _properties, bool _check = false);

	const std::string&	GetIP();
			bool		SetIP(const std::string& _ip, bool _check = false);

	const 	uint16_t	GetPort();
			bool		SetPort(uint16_t _port, bool _check = false);

	static	const	std::string&	Type();

protected:

	std::string	ip_;
	uint16_t	port_;
};

#endif

#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(ObjectManager& _manager, Type _type);
	DeviceIP(ObjectManager& _manager, Type _type, const ValueIP& _ip);

	virtual	bool		GetProperties(Properties& _properties) const;

	const 	ValueIP&	GetIP();
			void		SetIP(const ValueIP& _ip);
			bool		SetIP(const std::string& _ip);

protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);

	ValueIP		ip_;
};

#endif

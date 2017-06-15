#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(ObjectManager& _manager, ValueType const& _type);
	DeviceIP(ObjectManager& _manager, ValueType const& _type, const ValueIP& _ip);

			bool		IsIncludedIn(ValueType const& _type);

	virtual	bool		SetProperty(Property const& _property, Properties::Fields const& _fields = PROPERTY_ALL);

	virtual	bool		GetProperties(Properties& _properties, Properties::Fields const& _fields = PROPERTY_ALL);

	const 	ValueIP&	GetIP();
			void		SetIP(const ValueIP& _ip);
			bool		SetIP(const std::string& _ip);

	static	const	ValueType&	Type();

protected:

	ValueIP		ip_;
};

#endif

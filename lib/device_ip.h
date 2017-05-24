#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(ObjectManager& _manager, ValueType const& _type);
	DeviceIP(ObjectManager& _manager, ValueType const& _type, const ValueIP& _ip);

			bool		IsIncludedIn(ValueType const& _type);
	virtual	bool		GetProperties(Properties& _properties) const;

	const 	ValueIP&	GetIP();
			void		SetIP(const ValueIP& _ip, bool _store = true);
			bool		SetIP(const std::string& _ip, bool _store = true);

	static	const	ValueType&	Type();

protected:
	virtual	bool		SetPropertyInternal(Property const& _property, bool create = false);

	ValueIP		ip_;
};

#endif

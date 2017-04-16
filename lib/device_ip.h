#ifndef	DEVICE_IP_H_
#define	DEVICE_IP_H_

#include "device.h"

class	DeviceIP : public Device
{
public:
	DeviceIP(Type _type);
	DeviceIP(Type _type, const ValueIP& _ip);

	virtual	bool		GetProperties(Properties& _properties) const;
	virtual	bool		SetProperty(Property const& _property);

	const 	ValueIP&	GetIP();
			void		SetIP(const ValueIP& _ip);
			bool		SetIP(const std::string& _ip);

			void		Print(std::ostream& os) const;
protected:
	ValueIP		ip_;
};

#endif

#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_ip.h"

DeviceIP::DeviceIP()
: ip_(DEFAULT_LOCAL_IP)
{
}

DeviceIP::DeviceIP(const ValueIP& _ip)
: ip_(_ip)
{
}

const ValueIP&	DeviceIP::GetIP()
{
	return	ip_;
}

void	DeviceIP::SetIP(const ValueIP& _ip)
{
	ip_ = _ip;
}

bool	DeviceIP::SetIP(const std::string& _ip)
{
	if (ValueIP::IsValidIP(_ip))
	{
		ip_ = _ip;

		return	true;
	}

	return	false;
}

bool	DeviceIP::GetProperties(Properties& _properties)
{
	if (Device::GetProperties(_properties))
	{
		_properties.Append("ip", ip_);
		return	true;	
	}

	return	false;
}

bool	DeviceIP::SetProperty(Property const& _property)
{
	if (strcasecmp(_property.GetName().c_str(), "ip") == 0)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			return	ip_.Set(value->Get());
		}
	}
	else
	{
		return	DeviceIP::SetProperty(_property);
	}

	return	false;
}

void	DeviceIP::Print(std::ostream& os) const
{
	Device::Print(os);
	os << std::setw(16) << "IP Address : " << ip_ << std::endl;
}

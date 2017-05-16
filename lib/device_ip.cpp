#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_ip.h"

DeviceIP::DeviceIP(ObjectManager& _manager, Type _type)
: Device(_manager, _type), ip_(DEFAULT_LOCAL_IP)
{
}

DeviceIP::DeviceIP(ObjectManager& _manager, Type _type, const ValueIP& _ip)
: Device(_manager, _type), ip_(_ip)
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

bool	DeviceIP::GetProperties(Properties& _properties) const
{
	if (Device::GetProperties(_properties))
	{
		_properties.Append("ip", ip_);
		return	true;	
	}

	return	false;
}

bool	DeviceIP::SetPropertyInternal(Property const& _property, bool create)
{
	if (strcasecmp(_property.GetName().c_str(), "ip") == 0)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			TRACE_INFO("The ip set to " << value->Get());
			return	ip_.Set(value->Get());
		}
		else
		{
			TRACE_ERROR("Failed to set property because value types is not string!");
		}
	}
	else
	{
		return	Device::SetPropertyInternal(_property, create);
	}

	return	false;
}


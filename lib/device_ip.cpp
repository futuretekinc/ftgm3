#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_ip.h"

DeviceIP::DeviceIP(Type _type)
: Device(_type), ip_(DEFAULT_LOCAL_IP)
{
}

DeviceIP::DeviceIP(Type _type, const ValueIP& _ip)
: Device(_type), ip_(_ip)
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

bool	DeviceIP::SetProperty(Property const& _property, bool create)
{
	if (strcasecmp(_property.GetName().c_str(), "ip") == 0)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			TRACE_INFO << "The ip of object[" << id_ <<"] was set to " << value->Get() << Trace::End;
			return	ip_.Set(value->Get());
		}
		else
		{
			TRACE_ERROR	<< "Failed to set property because value types is not string!" << Trace::End;
		}
	}
	else
	{
		return	Device::SetProperty(_property, create);
	}

	return	false;
}

void	DeviceIP::Print(std::ostream& os) const
{
	Device::Print(os);
	os << std::setw(16) << "IP Address : " << ip_ << std::endl;
}

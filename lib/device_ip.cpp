#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_ip.h"

DeviceIP::DeviceIP(ObjectManager& _manager, ValueType const& _type)
: Device(_manager, _type), ip_(DEFAULT_LOCAL_IP)
{
}

DeviceIP::DeviceIP(ObjectManager& _manager, ValueType const& _type, const ValueIP& _ip)
: Device(_manager, _type), ip_(_ip)
{
}

const	ValueType&	DeviceIP::Type()
{
	static	ValueType	type_("device_ip");

	return	type_;
}

bool		DeviceIP::IsIncludedIn(ValueType const& _type)
{
	if (_type == DeviceIP::Type())
	{
		return	true;	
	}

	return	Device::IsIncludedIn(_type);
}

const ValueIP&	DeviceIP::GetIP()
{
	return	ip_;
}

void	DeviceIP::SetIP(const ValueIP& _ip, bool _store)
{
	ip_ = _ip;

	updated_properties_.AppendIP(ip_);
	if (_store)
	{
		ApplyChanges();	
	}
}

bool	DeviceIP::SetIP(const std::string& _ip, bool _store)
{
	if (ValueIP::IsValidIP(_ip))
	{
		ip_ = _ip;

		updated_properties_.AppendIP(ip_);
		if (_store)
		{
			ApplyChanges();	
		}

		return	true;
	}

	return	false;
}

bool	DeviceIP::GetProperties(Properties& _properties) const
{
	if (Device::GetProperties(_properties))
	{
		_properties.AppendIP(ip_);
		return	true;	
	}

	return	false;
}

bool	DeviceIP::SetPropertyInternal(Property const& _property, bool create)
{
	if (strcasecmp(_property.GetName().c_str(), TITLE_NAME_IP) == 0)
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


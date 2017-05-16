#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_serial.h"

DeviceSerial::DeviceSerial(ObjectManager& _manager, Type _type)
: Device(_manager, _type), port_(DEFAULT_LOCAL_IP)
{
}

DeviceSerial::DeviceSerial(ObjectManager& _manager, Type _type, std::string const& _port)
: Device(_manager, _type), port_(_port)
{
}

const std::string&	DeviceSerial::GetPort()
{
	return	port_;
}

bool	DeviceSerial::SetPort(std::string const& _port)
{
	port_ = _port;

	return	true;
}


bool	DeviceSerial::GetProperties(Properties& _properties) const
{
	if (Device::GetProperties(_properties))
	{
		_properties.Append("port", port_);
		return	true;	
	}

	return	false;
}

bool	DeviceSerial::SetPropertyInternal(Property const& _property, bool create)
{
	if (strcasecmp(_property.GetName().c_str(), "port") == 0)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			port_ = *value;

			return	true;
		}
		else
		{
			TRACE_ERROR("Failed to set " << _property.GetName() << " property because value type is invalid.");
			return	false;
		}
	}

	return	DeviceSerial::SetPropertyInternal(_property, create);
}

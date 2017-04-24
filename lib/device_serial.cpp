#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "device_serial.h"

DeviceSerial::DeviceSerial(Type _type)
: Device(_type), port_(DEFAULT_LOCAL_IP)
{
}

DeviceSerial::DeviceSerial(Type _type, std::string const& _port)
: Device(_type), port_(_port)
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

bool	DeviceSerial::SetProperty(Property const& _property)
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
			TRACE_ERROR << "Failed to set " << _property.GetName() << " property because value type is invalid." << Trace::End;
			return	false;
		}
	}

	return	DeviceSerial::SetProperty(_property);
}

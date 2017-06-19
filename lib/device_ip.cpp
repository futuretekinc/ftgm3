#include <strings.h>
#include <iomanip>
#include "defined.h"
#include "json.h"
#include "device_ip.h"
#include "utils.h"

DeviceIP::DeviceIP(ObjectManager& _manager, std::string const& _type)
: Device(_manager, _type), ip_(DEFAULT_CONST_LOCAL_IP)
{
}

DeviceIP::DeviceIP(ObjectManager& _manager, std::string const& _type, const std::string& _ip)
: Device(_manager, _type), ip_(_ip)
{
}

const	std::string&	DeviceIP::Type()
{
	static	std::string	type_("device_ip");

	return	type_;
}

bool		DeviceIP::IsIncludedIn(std::string const& _type)
{
	if (_type == DeviceIP::Type())
	{
		return	true;	
	}

	return	Device::IsIncludedIn(_type);
}

const std::string&	DeviceIP::GetIP()
{
	return	ip_;
}

bool	DeviceIP::SetIP(const std::string& _ip, bool _check)
{
	if (!IsValidIP(_ip))
	{
		TRACE_ERROR("Invalid IP!");
		return	false;
	}

	if (!_check)
	{
		ip_ = _ip;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_IP, ip_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

bool	DeviceIP::GetProperties(JSONNode& _properties, Fields const& _fields) 
{
	if (Device::GetProperties(_properties, _fields))
	{
		if(_fields.ip)
		{
			_properties.push_back(JSONNode(TITLE_NAME_IP, ip_));
		}

		return	true;	
	}

	return	false;
}

bool	DeviceIP::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_IP)
	{
		ret_value = SetIP(_property.as_string(), _check);
	}
	else
	{
		ret_value = Device::SetProperty(_property, _check);
	}

	return	ret_value;
}


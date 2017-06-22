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


bool	DeviceIP::GetOptions(JSONNode& _options)
{
	JSONNode	options(JSON_NODE);

	if (!Device::GetOptions(options))
	{
		TRACE_ERROR("Failed to get device options");
		return	false;	
	}

	options.push_back(JSONNode(TITLE_NAME_IP, ip_));

	_options = options;

	return	true;
}

bool	DeviceIP::SetOption(JSONNode const& _property, bool _check)
{
	bool	ret_value;

	if (_property.name() == TITLE_NAME_IP)
	{
		ret_value = SetIP(_property.as_string(), _check);
	}
	else
	{
		ret_value = Device::SetOption(_property, _check);	
	}

	return	ret_value;
}

#include <iostream>
#include "object_manager.h"
#include "device_fte.h"
#include "device_snmp.h"
#include "trace.h"

static const char*	class_name = "DeviceFTE";

DeviceFTE::DeviceFTE(ObjectManager& _manager)
: DeviceSNMP(_manager, DeviceFTE::Type(), "FTE-E")
{
	trace.SetClassName(class_name);
}

DeviceFTE::DeviceFTE(ObjectManager& _manager, JSONNode const& _properties)
: DeviceSNMP(_manager, DeviceFTE::Type(), _properties)
{
	trace.SetClassName(class_name);
}

DeviceFTE::DeviceFTE(ObjectManager& _manager, std::string const& _ip)
: DeviceSNMP(_manager, DeviceFTE::Type(), "FTE-E",  _ip)
{
	trace.SetClassName(class_name);
}

const char*	DeviceFTE::GetClassName()
{
	return	class_name;
}

bool		DeviceFTE::IsIncludedIn(std::string const& _type)
{
	if (_type == DeviceFTE::Type())
	{
		return	true;	
	}

	return	DeviceSNMP::IsIncludedIn(_type);
}


std::string* DeviceFTE::Parsing(std::string strOrigin, std::string strTok, int size)
{
	int cutAt;
  	int index = 0;
	std::string* strResult = new std::string[size];
	
	while((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if(cutAt > 0)
		{
			strResult[index++] = strOrigin.substr(0,cutAt);
		}
	       	strOrigin = strOrigin.substr(cutAt + 1);
    	}

	if(strOrigin.length() > 0)
  	{
  		strResult[index++] = strOrigin.substr(0, cutAt);

     	}
     	return strResult;
}

bool DeviceFTE::ValueParsing(std::string& _value)
{
	std::string value_temp;
	std::string etc = "\"{}:,";
	std::string* rev_value = Parsing(_value, etc, 6);
	
	
	
	if(rev_value[0].compare("T") != 0)
	{
		return false;
	}
	if(rev_value[2].compare("C") != 0)
	{
		return false;
	} 
	if(rev_value[4].compare("H") != 0)
	{
		return false;
	}
	_value = rev_value[1] + "," + rev_value[3] + "," + rev_value[5];
	return true;
}

Endpoint*	DeviceFTE::CreateEndpoint(JSONNode const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

bool	DeviceFTE::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	bool	ret_value = false;

	Endpoint*	endpoint = manager_.GetEndpoint(_epid);
	if (endpoint != NULL)
	{
		SNMP::OID oid = GetOID(endpoint->GetType(), endpoint->GetSensorID());
		
		ret_value = DeviceSNMP::ReadValue(oid, _time, _value);
		
		if(endpoint->GetType() == OBJECT_TYPE_EP_A_FX3DTEMPCTR)
		{
			ret_value = ValueParsing(_value);
		}

		if (ret_value)
		{
			TRACE_INFO("The endpoint[" << std::string(oid) << "] : " << _value);	
		}
		else
		{
			TRACE_INFO("Failed to read value from The endpoint[" << std::string(oid));
		}
	}
	else
	{
		TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
	}

	return	ret_value;
}

bool	DeviceFTE::WriteValue(std::string const& _epid, std::string const& _value)
{
	bool	ret_value = false;

	Endpoint*	endpoint = manager_.GetEndpoint(_epid);
	if (endpoint != NULL)
	{
		SNMP::OID oid = GetOID(endpoint->GetType(), endpoint->GetSensorID());

		ret_value = DeviceSNMP::WriteValue(oid, _value);
		if (ret_value)
		{
			TRACE_INFO("The endpoint[" << std::string(oid) << "] : " << _value);	
		}
		else
		{
			TRACE_INFO("Failed to write value from The endpoint[" << std::string(oid) << "]");
		}
	}
	else
	{
		TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
	}

	return	ret_value;
}

bool	DeviceFTE::WriteValue(std::string const& _epid, bool _value)
{
	bool	ret_value = false;

	Endpoint*	endpoint = manager_.GetEndpoint(_epid);
	if (endpoint != NULL)
	{
		SNMP::OID oid = GetOID(endpoint->GetType(), endpoint->GetSensorID());

		ret_value = DeviceSNMP::WriteValue(oid, _value);
		if (ret_value)
		{
			TRACE_INFO("The endpoint[" << std::string(oid) << "] : " << _value);	
		}
		else
		{
			TRACE_INFO("Failed to write value from The endpoint[" << std::string(oid) << "]");
		}
	}
	else
	{
		TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
	}

	return	ret_value;
}

SNMP::OID DeviceFTE::GetOID(std::string const& _type, uint32_t _index)
{
	SNMP::OID oid;
	uint32_t	type_index = 0;

	if (_type == OBJECT_TYPE_EP_S_TEMPERATURE)
	{
		type_index = 1;
	}
	else if ((_type == OBJECT_TYPE_EP_S_HUMIDITY) || (_type == OBJECT_TYPE_EP_S_SOIL_MOISTURE))
	{
		type_index = 2;
	}
	else if (_type == OBJECT_TYPE_EP_S_VOLTAGE)
	{
		type_index = 3;
	}
	else if (_type == OBJECT_TYPE_EP_S_CURRENT)
	{
		type_index = 4;
	}
	else if (_type == OBJECT_TYPE_EP_S_DI)
	{
		type_index = 5;
	}
	else if (_type == OBJECT_TYPE_EP_A_DO)
	{
		type_index = 6;
	}
	else if ((_type == OBJECT_TYPE_EP_S_PRESSURE) || (_type == OBJECT_TYPE_EP_S_GAS))
	{
		type_index = 7;
	}
	else if ((_type == OBJECT_TYPE_EP_S_WIND_SPEED) || (_type == OBJECT_TYPE_EP_S_RAINFALL) || (_type == OBJECT_TYPE_EP_S_SOIL_ACIDITY))
	{
		type_index = 10;
	}
	else if (_type == OBJECT_TYPE_EP_A_FX3DTEMPCTR)
	{
		type_index = 127;
	}

	oid.id[oid.length++] = 1;
	oid.id[oid.length++] = 3;
	oid.id[oid.length++] = 6;
	oid.id[oid.length++] = 1;
	oid.id[oid.length++] = 4;
	oid.id[oid.length++] = 1;
	oid.id[oid.length++] = 42251;
	oid.id[oid.length++] = 1;
	oid.id[oid.length++] = 3;
	oid.id[oid.length++] = (type_index << 8);
	oid.id[oid.length++] = 2;
	oid.id[oid.length++] = 1;
	oid.id[oid.length++] = 6;
	oid.id[oid.length++] = _index + 1;

	return	oid;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const	char*	DeviceFTE::Type()
{
	return	OBJECT_TYPE_DEV_FTE;
}


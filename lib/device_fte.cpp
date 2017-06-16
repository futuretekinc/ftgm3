#include <iostream>
#include "object_manager.h"
#include "device_fte.h"
#include "device_snmp.h"
#include "trace.h"

DeviceFTE::DeviceFTE(ObjectManager& _manager)
: DeviceSNMP(_manager, DeviceFTE::Type(), "FTE-E")
{
	trace.SetClassName(GetClassName());
}

DeviceFTE::DeviceFTE(ObjectManager& _manager, Properties const& _properties)
: DeviceSNMP(_manager, DeviceFTE::Type(), _properties)
{
	trace.SetClassName(GetClassName());
}

DeviceFTE::DeviceFTE(ObjectManager& _manager, JSONNode const& _properties)
: DeviceSNMP(_manager, DeviceFTE::Type(), _properties)
{
	trace.SetClassName(GetClassName());
}

DeviceFTE::DeviceFTE(ObjectManager& _manager, ValueIP const& _ip)
: DeviceSNMP(_manager, DeviceFTE::Type(), "FTE-E",  _ip)
{
	trace.SetClassName(GetClassName());
}

bool		DeviceFTE::IsIncludedIn(ValueType const& _type)
{
	if (_type == DeviceFTE::Type())
	{
		return	true;	
	}

	return	DeviceSNMP::IsIncludedIn(_type);
}

Endpoint*	DeviceFTE::CreateEndpoint(Properties const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

bool	DeviceFTE::Attach(ValueID const& _endpoint_id)
{
	Endpoint*	endpoint = manager_.GetEndpoint(std::string(_endpoint_id));
	if (endpoint == NULL)
	{
		return	false;
	}

	std::string	sensor_id = endpoint->GetSensorID();

	if (sensor_id.size() < 8)
	{
		TRACE_ERROR("Failed to attach endpoint because sensor id[" << sensor_id << "] is invalid!");
		return	false;
	}

	uint32_t	index = strtoul(sensor_id.substr(sensor_id.size() - 2, 2).c_str(), 0, 16) - 1;
	if (index > 255)
	{
		TRACE_ERROR("Failed to attach endpoint because sensor id is invalid!");
		return	false;
	}

	auto endpoint_it = endpoint_table_.find(_endpoint_id);
	if (endpoint_it == endpoint_table_.end())
	{
		endpoint_table_[_endpoint_id] = index;
	}

	return	Device::Attach(_endpoint_id);
}

bool	DeviceFTE::Detach(ValueID const& _endpoint_id)
{
	Endpoint*	endpoint = manager_.GetEndpoint(std::string(_endpoint_id));
	if (endpoint == NULL)
	{
		return	false;
	}

	std::string	sensor_id = endpoint->GetSensorID();

	if (sensor_id.size() < 8)
	{
		TRACE_ERROR("Failed to attach endpoint because sensor id is invalid!");
		return	false;
	}

	uint32_t	index = strtoul(sensor_id.substr(sensor_id.size() - 2, 2).c_str(), 0, 16) - 1;
	if (index > 255)
	{
		TRACE_ERROR("Failed to attach endpoint because sensor id is invalid!");
		return	false;
	}

	auto endpoint_it = endpoint_table_.find(_endpoint_id);
	if (endpoint_it == endpoint_table_.end())
	{
		TRACE_INFO("The device[" << _endpoint_id << "] is not attached.");
		return	false;
	}

	endpoint_table_.erase(endpoint_it);

	return	Device::Detach(_endpoint_id);
}

bool	DeviceFTE::ReadValue(std::string const& _endpoint_id, Value* _value)
{
	Endpoint*	endpoint = manager_.GetEndpoint(_endpoint_id);
	if (endpoint == NULL)
	{
		TRACE_ERROR("The endpoint[" << _endpoint_id << "] is not attached");
		return	false;	
	}

	auto it = endpoint_table_.find(_endpoint_id);
	if (it == endpoint_table_.end())
	{
		TRACE_ERROR("The endpoint[" << _endpoint_id << "] has been abnormally attached");
		return	false;	
	}

	OID oid = GetOID(endpoint->GetType(), it->second);

	return	DeviceSNMP::ReadValue(oid, _value);
}

DeviceSNMP::OID DeviceFTE::GetOID(std::string const& _id)
{
	if (_id.size() < 8)
	{
		return	oid_map_[""];
	}

	if (DeviceSNMP::GetOID(_id).length == 0)
	{

		std::string type_string = _id.substr(_id.size() - 8, 2);
		std::string index_string= _id.substr(_id.size() - 2, 2);

		uint32_t	type = strtoul(type_string.c_str(), 0, 16) & 0x3F;
		uint32_t	index = strtoul(index_string.c_str(), 0, 16);

		OID oid;
		
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 3;
		oid.id[oid.length++] = 6;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 4;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 42251;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 3;
		oid.id[oid.length++] = (type << 8);
		oid.id[oid.length++] = 2;
		oid.id[oid.length++] = 1;
		oid.id[oid.length++] = 6;
		oid.id[oid.length++] = index;

		oid_map_[_id] = oid;
	}

	TRACE_INFO("GetOID(" << _id << ") = " << std::string(oid_map_[_id]));

	return	oid_map_[_id];
}

DeviceSNMP::OID DeviceFTE::GetOID(ValueType const& _type, uint32_t _index)
{
	OID oid;
	uint32_t	type_index = 0;

	if (_type == NODE_TYPE_EP_S_TEMPERATURE)
	{
		type_index = 1;
	}
	else if (_type == NODE_TYPE_EP_S_HUMIDITY)
	{
		type_index = 2;
	}
	else if (_type == NODE_TYPE_EP_S_VOLTAGE)
	{
		type_index = 3;
	}
	else if (_type == NODE_TYPE_EP_S_CURRENT)
	{
		type_index = 4;
	}
	else if (_type == NODE_TYPE_EP_S_DI)
	{
		type_index = 5;
	}
	else if (_type == NODE_TYPE_EP_A_DO)
	{
		type_index = 6;
	}
	else if (_type == NODE_TYPE_EP_S_PRESSURE)
	{
		type_index = 7;
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

	TRACE_INFO("GetOID(" << _type <<", " << _index << ") = " << std::string(oid));
	return	oid;
}

const	ValueType&	DeviceFTE::Type()
{
	static	ValueType	type_(NODE_TYPE_DEV_FTE);

	return	type_;
}


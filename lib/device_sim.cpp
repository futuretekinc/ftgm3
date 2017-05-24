#include <iostream>
#include "object_manager.h"
#include "device_sim.h"
#include "device_snmp.h"
#include "trace.h"

DeviceSIM::DeviceSIM(ObjectManager& _manager)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E")
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, Properties const& _properties)
: DeviceSNMP(_manager, DeviceSIM::Type(), _properties)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, ValueIP const& _ip)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E",  _ip)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
}

bool		DeviceSIM::IsIncludedIn(ValueType const& _type)
{
	if (_type == DeviceSIM::Type())
	{
		return	true;	
	}

	return	DeviceSNMP::IsIncludedIn(_type);
}

Endpoint*	DeviceSIM::CreateEndpoint(Properties const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

bool	DeviceSIM::Attach(ValueID const& _endpoint_id)
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

bool	DeviceSIM::Detach(ValueID const& _endpoint_id)
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

bool	DeviceSIM::ReadValue(std::string const& _endpoint_id, Value* _value)
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


	ValueFloat	*value_float = dynamic_cast<ValueFloat*>(_value);
	if (value_float != NULL)
	{
		value_float->Set((rand() % 10000) / 100.0);
	}

	return	true;
}

DeviceSNMP::OID DeviceSIM::GetOID(std::string const& _id)
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
		oid.id[oid.length++] = 0;

		oid_map_[_id] = oid;
	}

	TRACE_INFO("GetOID(" << _id << ") = " << std::string(oid_map_[_id]));

	return	oid_map_[_id];
}

DeviceSNMP::OID DeviceSIM::GetOID(Endpoint::Type _type, uint32_t _index)
{
	OID oid;
	uint32_t	type_index = 0;

	switch(_type)
	{
	case	Endpoint::S_TEMPERATURE: 	type_index = 1; break;
	case	Endpoint::S_HUMIDITY:		type_index = 2; break;
	case	Endpoint::S_VOLTAGE:		type_index = 3; break;
	case	Endpoint::S_CURRENT:		type_index = 4; break;
	case	Endpoint::S_DI:				type_index = 5; break;
	case	Endpoint::A_DO:				type_index = 6; break;
	case	Endpoint::S_PRESSURE:		type_index = 7; break;
	};

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
	oid.id[oid.length++] = 0;

	TRACE_INFO("GetOID(" << _type <<", " << _index << ") = " << std::string(oid));
	return	oid;
}

const	ValueType&	DeviceSIM::Type()
{
	static	ValueType	type_("d_sim");

	return	type_;
}


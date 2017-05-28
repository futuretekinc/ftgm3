#include <iostream>
#include "object_manager.h"
#include "device_sim.h"
#include "device_snmp.h"
#include "trace.h"

DeviceSIM::DeviceSIM(ObjectManager& _manager)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E")
{
	trace.SetClassName(GetClassName());
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, Properties const& _properties)
: DeviceSNMP(_manager, DeviceSIM::Type(), _properties)
{
	trace.SetClassName(GetClassName());
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, ValueIP const& _ip)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E",  _ip)
{
	trace.SetClassName(GetClassName());
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

	ValueFloat	*value_float = dynamic_cast<ValueFloat*>(_value);
	if (value_float != NULL)
	{
		value_float->Set((rand() % 10000) / 100.0);
	}

	return	true;
}

const	ValueType&	DeviceSIM::Type()
{
	static	ValueType	type_("d_sim");

	return	type_;
}


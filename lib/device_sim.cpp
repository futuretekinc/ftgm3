#include <iostream>
#include "object_manager.h"
#include "device_sim.h"
#include "device_snmp.h"
#include "trace.h"
#include "endpoint_sensor.h"
#include "endpoint_actuator.h"

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

DeviceSIM::DeviceSIM(ObjectManager& _manager, JSONNode const& _properties)
: DeviceSNMP(_manager, DeviceSIM::Type(), _properties)
{
	trace.SetClassName(GetClassName());
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, ValueIP const& _ip)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E",  _ip)
{
	trace.SetClassName(GetClassName());
}

const	ValueType&	DeviceSIM::Type()
{
	static	ValueType	type_("d_sim");

	return	type_;
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

	if (dynamic_cast<EndpointSensor*>(endpoint))
	{
		auto it = endpoint_sensor_value_table_.find(_endpoint_id);
		if (it == endpoint_sensor_value_table_.end())
		{
			endpoint_sensor_value_table_[_endpoint_id] = index * 10;
		}
	}
	else if (dynamic_cast<EndpointActuator*>(endpoint))
	{
		auto it = endpoint_actuator_value_table_.find(_endpoint_id);
		if (it == endpoint_actuator_value_table_.end())
		{
			endpoint_actuator_value_table_[_endpoint_id] = ENDPOINT_ACTUATOR_DO_OFF;
		}
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

	auto it = endpoint_sensor_value_table_.find(_endpoint_id);
	if (it != endpoint_sensor_value_table_.end())
	{
		endpoint_sensor_value_table_.erase(it);
	}
	else 
	{
		auto it = endpoint_actuator_value_table_.find(_endpoint_id);
		if (it != endpoint_actuator_value_table_.end())
		{
			endpoint_actuator_value_table_.erase(it);
		}
		else
		{
			TRACE_INFO("The device[" << _endpoint_id << "] is not attached.");
			return	false;
		}
	}


	return	Device::Detach(_endpoint_id);
}

bool	DeviceSIM::ReadValue(std::string const& _endpoint_id, std::string& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_endpoint_id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("The endpoint[" << _endpoint_id << "] is not attached");
			return	false;	
		}

		if (dynamic_cast<EndpointSensor*>(endpoint))
		{
			auto it = endpoint_sensor_value_table_.find(_endpoint_id);
			if (it == endpoint_sensor_value_table_.end())
			{
				TRACE_ERROR("The endpoint[" << _endpoint_id << "] has been abnormally attached");
				return	false;	
			}

			it->second = it->second + ((rand() % 20000) - 10000) / 10000.0;

			_value = std::to_string(it->second);
		}
		else if (dynamic_cast<EndpointActuator*>(endpoint))
		{
			auto it = endpoint_actuator_value_table_.find(_endpoint_id);
			if (it == endpoint_actuator_value_table_.end())
			{
				TRACE_ERROR("The endpoint[" << _endpoint_id << "] has been abnormally attached");
				return	false;	
			}

			_value = it->second;
		}
		else
		{
			TRACE_ERROR("Invalid endpoint type!");
			return	false;	
		}
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Exception : " << e.what());	
	}
	return	true;
}

bool	DeviceSIM::WriteValue(std::string const& _endpoint_id, std::string const& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_endpoint_id);
		if (endpoint == NULL)
		{
			TRACE_ERROR("The endpoint[" << _endpoint_id << "] is not attached");
			return	false;	
		}

		if (dynamic_cast<EndpointActuator*>(endpoint))
		{
			auto it = endpoint_actuator_value_table_.find(_endpoint_id);
			if (it == endpoint_actuator_value_table_.end())
			{
				TRACE_ERROR("The endpoint[" << _endpoint_id << "] has been abnormally attached");
				return	false;	
			}

			TRACE_INFO("The endpoint[" << _endpoint_id << "] is set to " << _value);
			it->second = _value;
		}
		else
		{
			TRACE_ERROR("Endpoint Sensor is not supported to set!");
		}

	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Exception : " << e.what());	
	}
	return	true;
}


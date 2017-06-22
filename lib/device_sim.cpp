#include <iostream>
#include "exception.h"
#include "object_manager.h"
#include "device_sim.h"
#include "device_snmp.h"
#include "trace.h"
#include "utils.h"
#include "endpoint_sensor.h"
#include "endpoint_sensor_linear.h"
#include "endpoint_sensor_discrete.h"
#include "endpoint_actuator.h"
#include "endpoint_actuator_discrete.h"

DeviceSIM::DeviceSIM(ObjectManager& _manager)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E")
{
	trace.SetClassName(GetClassName());
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, JSONNode const& _properties)
: DeviceSNMP(_manager, DeviceSIM::Type(), _properties)
{
	trace.SetClassName(GetClassName());
}

DeviceSIM::DeviceSIM(ObjectManager& _manager, std::string const& _ip)
: DeviceSNMP(_manager, DeviceSIM::Type(), "SIM-E",  _ip)
{
	trace.SetClassName(GetClassName());
}

const	std::string&	DeviceSIM::Type()
{
	static	std::string type_(NODE_TYPE_DEV_SIM);

	return	type_;
}

bool		DeviceSIM::IsIncludedIn(std::string const& _type)
{
	if (_type == DeviceSIM::Type())
	{
		return	true;	
	}

	return	DeviceSNMP::IsIncludedIn(_type);
}

Endpoint*	DeviceSIM::CreateEndpoint(JSONNode const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}

bool	DeviceSIM::Attach(std::string const& _epid)
{
	Endpoint*	endpoint = manager_.GetEndpoint(std::string(_epid));
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
		std::map<std::string, double>::iterator it = endpoint_sensor_value_table_.find(_epid);
		if (it == endpoint_sensor_value_table_.end())
		{
			endpoint_sensor_value_table_[_epid] = index * 10;
		}
	}
	else if (dynamic_cast<EndpointActuator*>(endpoint))
	{
		std::map<std::string, std::string>::iterator it = endpoint_actuator_value_table_.find(_epid);
		if (it == endpoint_actuator_value_table_.end())
		{
			endpoint_actuator_value_table_[_epid] = ENDPOINT_ACTUATOR_DO_OFF;
		}
	}

	return	Device::Attach(_epid);
}

bool	DeviceSIM::Detach(std::string const& _epid)
{
	Endpoint*	endpoint = manager_.GetEndpoint(_epid);
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

	std::map<std::string, double>::iterator  it = endpoint_sensor_value_table_.find(_epid);
	if (it != endpoint_sensor_value_table_.end())
	{
		endpoint_sensor_value_table_.erase(it);
	}
	else 
	{
		std::map<std::string, std::string>::iterator it = endpoint_actuator_value_table_.find(_epid);
		if (it != endpoint_actuator_value_table_.end())
		{
			endpoint_actuator_value_table_.erase(it);
		}
		else
		{
			TRACE_INFO("The device[" << _epid << "] is not attached.");
			return	false;
		}
	}


	return	Device::Detach(_epid);
}

bool	DeviceSIM::ReadValue(std::string const& _epid, time_t& time, std::string& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_epid);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] is not attached");
		}

		if (dynamic_cast<EndpointSensorLinear*>(endpoint))
		{
			std::map<std::string, double>::iterator it = endpoint_sensor_value_table_.find(_epid);
			if (it == endpoint_sensor_value_table_.end())
			{
				THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] has been abnormally attached");
			}

			double max = strtod(endpoint->GetValueMax().c_str(), 0);
			double min = strtod(endpoint->GetValueMin().c_str(), 0);
			double value = it->second + ((max - min) * 0.05 * (100 - (rand() % 200)) / 100.0);
			if (value > max)
			{
				value = max;
			}
			else if (value < min)
			{
				value = min;
			}
			
			it->second = value;

			_value = ::ToString(value, 2);

		}
		else if (dynamic_cast<EndpointSensorDiscrete*>(endpoint))
		{
			std::map<std::string, double>::iterator it = endpoint_sensor_value_table_.find(_epid);
			if (it == endpoint_sensor_value_table_.end())
			{
				THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] has been abnormally attached");
			}

			it->second = (rand() % 2);

			_value = ::ToString(it->second, 0);
		}
		else if (dynamic_cast<EndpointActuatorDiscrete*>(endpoint))
		{
			std::map<std::string, std::string>::iterator it = endpoint_actuator_value_table_.find(_epid);
			if (it == endpoint_actuator_value_table_.end())
			{
				THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] has been abnormally attached");
			}

			_value = it->second;
		}
		else
		{
			TRACE_ERROR("Invalid endpoint type!");
			return	false;	
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
	catch(std::exception& e)
	{
		TRACE_ERROR("Exception : " << e.what());	
		return	false;
	}

	time = Date::GetCurrent();

	return	true;
}

bool	DeviceSIM::WriteValue(std::string const& _epid, std::string const& _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_epid);
		if (endpoint == NULL)
		{
			TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
			return	false;	
		}

		if (dynamic_cast<EndpointActuator*>(endpoint))
		{
			std::map<std::string, std::string>::iterator it = endpoint_actuator_value_table_.find(_epid);
			if (it == endpoint_actuator_value_table_.end())
			{
				TRACE_ERROR("The endpoint[" << _epid << "] has been abnormally attached");
				return	false;	
			}

			TRACE_INFO("The endpoint[" << _epid << "] is set to " << _value);
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

bool	DeviceSIM::WriteValue(std::string const& _epid, bool _value)
{
	try
	{
		Endpoint*	endpoint = manager_.GetEndpoint(_epid);
		if (endpoint == NULL)
		{
			TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
			return	false;	
		}

		if (dynamic_cast<EndpointActuator*>(endpoint))
		{
			std::map<std::string, std::string>::iterator it = endpoint_actuator_value_table_.find(_epid);
			if (it == endpoint_actuator_value_table_.end())
			{
				TRACE_ERROR("The endpoint[" << _epid << "] has been abnormally attached");
				return	false;	
			}

			TRACE_INFO("The endpoint[" << _epid << "] is set to " << _value);
			it->second = ToString(_value);
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


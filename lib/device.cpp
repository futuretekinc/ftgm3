#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "exception.h"
#include "json.h"
#include "trace.h"
#include "device.h"
#include "object_manager.h"
#include "device_snmp.h"
#include "device_fte.h"
#include "device_sim.h"
#include "endpoint.h"
#include "endpoint_sensor.h"

Device::Device(ObjectManager& _manager, ValueType const& _type)
:	Node(_manager, _type)
{
}

Device::~Device()
{
}

bool	Device::GetProperties(Properties& _properties, Properties::Fields const& _fields) 
{
	if (Node::GetProperties(_properties, _fields))
	{
#if 0
		if (_child)
		{
			PropertiesList	endpoint_properties_list;

			for(auto it = endpoint_id_list_.begin() ; it != endpoint_id_list_.end() ; it++)
			{
				Properties	endpoint_properties;

				Endpoint*endpoint = manager_.GetEndpoint(*it);
				if (endpoint != NULL)
				{
					endpoint->GetProperties(endpoint_properties, _fields, _child);	
					endpoint_properties_list.Append(endpoint_properties);	
				}
			}

			if (endpoint_properties_list.size() != 0)
			{
				_properties.Append(Property(TITLE_NAME_ENDPOINT, endpoint_properties_list));
			}
		}
#endif
		return	true;	
	}

	return	false;
}

bool	Device::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	return	Node::GetProperties(_properties, _fields);
}

bool	Device::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	bool	ret_value = true;

	if (_property.GetName() == TITLE_NAME_ENDPOINT)
	{
	}
	else
	{
		ret_value = Node::SetProperty(_property, _fields);
	}

	return	ret_value;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Device::IsAttached(ValueID const& _endpoint_id)
{
	for(auto it = endpoint_id_list_.begin() ; it != endpoint_id_list_.end() ; it++)
	{
		if ((*it) == _endpoint_id)
		{	
			return	true;
		}
	}

	return	false;
}

bool	Device::Attach(ValueID const& _endpoint_id)
{
	if (!IsAttached(_endpoint_id))
	{
		endpoint_id_list_.push_back(_endpoint_id);
		TRACE_INFO("The endpoint[" << _endpoint_id << "] has been attahced.");
	}

	return	true;
}

bool	Device::Detach(ValueID const& _endpoint_id)
{
	for(auto it = endpoint_id_list_.begin() ; it != endpoint_id_list_.end() ; it++)
	{
		if ((*it) == _endpoint_id)
		{	
			endpoint_id_list_.erase(it);
			TRACE_INFO("The endpoint[" << _endpoint_id << "] has been detahced.");
			return	true;
		}
	}


	return	false;
}

bool	Device::Detach()
{
	endpoint_id_list_.clear();
	return	true;
}

uint32_t	Device::GetEndpointCount()
{
	return	endpoint_id_list_.size();
}

const ValueID&	Device::GetEndpointAt(int index)
{
	static	ValueID	null_id("");

	if (index >= 0)
	{
		for(auto it = endpoint_id_list_.begin() ; it != endpoint_id_list_.end() ; it++)
		{
			if (index == 0)
			{	
				return	*it;
			}

			index--;
		}
	}

	return	null_id;
}

bool		Device::GetEndpointList(std::list<ValueID>& _endpoint_id_list)
{
	_endpoint_id_list = endpoint_id_list_;

	return	true;
}

bool		Device::GetEndpointMap(std::map<std::string, Endpoint*>& _map)
{
	for(auto it = endpoint_id_list_.begin(); it != endpoint_id_list_.end(); it++)
	{
		Endpoint* endpoint = manager_.GetEndpoint(*it);
		if (endpoint != NULL)
		{
			_map[*it] = endpoint;
		}
	}

	return	true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

Device::operator JSONNode()
{
	JSONNode	properties;

	Object::GetProperties(properties);

#if 0
	PropertiesList	properties_list;

	for(auto it = endpoint_id_list_.begin(); it != endpoint_id_list_.end() ; it++)
	{
		Properties	endpoint_properties;
		Endpoint*	endpoint = manager_.GetEndpoint(std::string(*it));
		if (endpoint != NULL)
		{
			endpoint->GetProperties(endpoint_properties);
			properties_list.Append(endpoint_properties);
		}
	}

	properties.Append(Property(TITLE_NAME_ENDPOINT, properties_list));
#endif
	return	properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	Node::Preprocess();

	for(auto it = endpoint_id_list_.begin(); it != endpoint_id_list_.end(); it++)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(*it);
		if (endpoint != NULL)
		{
			endpoint->Start();	
		}
		else
		{
			TRACE_ERROR("Endpoint[" << (*it) << "] not found!");	
		}
	}
}

void	Device::Process()
{
	ValueID id;
	Timer	timer;
	bool	found = false;


	auto it = endpoint_schedule_list_.begin();
	if (it != endpoint_schedule_list_.end())
	{
		if (it->second.RemainTime() == 0)
		{
			id = it->first;
			timer = it->second;
			found = true;	
			endpoint_schedule_list_.erase(it);
		}
	}

	if (found)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(std::string(id));
		if (endpoint != NULL)
		{
			endpoint->CorrectionProcess();

			timer += Time(endpoint->GetCorrectionInterval() * TIME_SECOND);	

			AddSchedule(id, timer);
		}
	}

	Node::Process();
}

void	Device::Postprocess()
{
	for(auto it = endpoint_id_list_.begin(); it != endpoint_id_list_.end(); it++)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(std::string(*it));
		if (endpoint != NULL)
		{
			endpoint->Stop();	
		}
	}

	Node::Postprocess();

}

bool	Device::AddSchedule(ValueID const& _id, Timer const& _timer)
{
	bool	last = true;

	if (!IsAttached(_id))
	{
		TRACE_INFO("The endpoint[" << _id << "] is not attached");
		if (!Attach(_id) )
		{
			TRACE_INFO("Failed to attach the endpoint[" << _id << "]");
			return	false;
		}
	}

	endpoint_schedule_list_lock_.lock();

	for(auto it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->second.RemainTime() > _timer.RemainTime())
		{
			endpoint_schedule_list_.insert(it, std::pair<ValueID, Timer>(_id, _timer));	
			last = false;
			break;
		}
	}

	if (last)
	{
		endpoint_schedule_list_.push_back(std::pair<ValueID, Timer>(_id, _timer));	
	}

	endpoint_schedule_list_lock_.unlock();

	return	true;
}

bool	Device::RemoveSchedule(ValueID const& _id)
{
	bool	removed = false;
	endpoint_schedule_list_lock_.lock();

	for(auto it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->first == _id)
		{
			endpoint_schedule_list_.erase(it);
			removed = true;
			break;
		}
	}

	endpoint_schedule_list_lock_.unlock();

	return	removed;
}

bool	Device::IsValidType(ValueType const& _type)
{
	return	(std::string(_type) == std::string(DeviceSNMP::Type())) 
			|| (std::string(_type) == std::string(DeviceFTE::Type()))
			|| (std::string(_type) == std::string(DeviceSIM::Type()));
}

Device*	Device::Create(ObjectManager& _manager, Properties const& _properties)
{
	Device*	device = NULL;
	const Property *type_property = _properties.Get(TITLE_NAME_TYPE);

	if (type_property != NULL)
	{
		const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
		if (type_value != NULL)
		{
			Properties	properties(_properties);

			properties.Delete(TITLE_NAME_TYPE);

			if (std::string(*type_value) == std::string(DeviceSNMP::Type()))
			{
				device = new DeviceSNMP(_manager, properties);
			}
			else if (std::string(*type_value) == std::string(DeviceFTE::Type()))
			{
				device = new DeviceFTE(_manager, properties);
			}
			else if (std::string(*type_value) == std::string(DeviceSIM::Type()))
			{
				device = new DeviceSIM(_manager, properties);
			}
			else
			{
				TRACE_ERROR2(NULL, "Failed to create device. Device type[" << type_value->Get() << "] is not supported!");
			}
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create device. Device type is invalid!");
		}
	}
	else
	{
		TRACE_ERROR2(NULL, "Failed to create device. Device type unknown!");
	}

	return	device;
}

Device*	Device::Create(ObjectManager& _manager, JSONNode const& _properties)
{
	Device*	device = NULL;
	try
	{
		std::string	type = JSONNodeGetType(_properties);

		if (type == std::string(DeviceSNMP::Type()))
		{
			device = new DeviceSNMP(_manager, _properties);
		}
		else if (type == std::string(DeviceFTE::Type()))
		{
			device = new DeviceFTE(_manager, _properties);
		}
		else if (type == std::string(DeviceSIM::Type()))
		{
			device = new DeviceSIM(_manager, _properties);
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create device. Device type[" << type << "] is not supported!");
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR2(NULL, "Failed to create device. Device type unknown!");
	}

	return	device;
}


bool	Device::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Node::GetPropertyFieldList(_field_list))
	{
		_field_list.push_back(TITLE_NAME_IP);
		_field_list.push_back(TITLE_NAME_MODULE);
		_field_list.push_back(TITLE_NAME_COMMUNITY);
		_field_list.push_back(TITLE_NAME_TIMEOUT);
	}

	return	true;
}

bool	Device::WriteValue(std::string const& _endpoint_id, std::string const& _value)
{
	return	false;	
}

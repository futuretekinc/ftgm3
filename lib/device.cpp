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

Device::Device(ObjectManager& _manager, std::string const& _type)
:	Node(_manager, _type)
{
}

Device::~Device()
{
}

bool	Device::GetProperties(JSONNode& _properties, Fields const& _fields) 
{
	return	Node::GetProperties(_properties, _fields);
}


bool	Device::SetProperty(JSONNode const& _property, bool _check)
{
	return	Node::SetProperty(_property, _check);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Device::IsAttached(std::string const& _epid)
{
	for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
	{
		if ((*it) == _epid)
		{	
			return	true;
		}
	}

	return	false;
}

bool	Device::Attach(std::string const& _epid)
{
	if (!IsAttached(_epid))
	{
		epid_list_.push_back(_epid);
		TRACE_INFO("The endpoint[" << _epid << "] has been attahced.");
	}

	return	true;
}

bool	Device::Detach(std::string const& _epid)
{
	for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
	{
		if ((*it) == _epid)
		{	
			epid_list_.erase(it);
			TRACE_INFO("The endpoint[" << _epid << "] has been detahced.");
			return	true;
		}
	}


	return	false;
}

bool	Device::Detach()
{
	epid_list_.clear();
	return	true;
}

uint32_t	Device::GetEndpointCount()
{
	return	epid_list_.size();
}

const std::string&	Device::GetEndpointAt(int index)
{
	if (index >= 0)
	{
		for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
		{
			if (index == 0)
			{	
				return	*it;
			}

			index--;
		}
	}

	static	std::string null_id("");
	return	null_id;
}

bool		Device::GetEndpointList(std::list<std::string>& _epid_list)
{
	_epid_list = epid_list_;

	return	true;
}

bool		Device::GetEndpointMap(std::map<std::string, Endpoint*>& _map)
{
	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
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

	return	properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	Node::Preprocess();

	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
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
	std::string id;
	Timer	timer;
	bool	found = false;


	for(std::map<std::string, Timer>::iterator it = endpoint_schedule_list_.begin(); it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->second.RemainTime() == 0)
		{
			id = it->first;
			timer = it->second;
			found = true;	
			endpoint_schedule_list_.erase(it);
			break;
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
	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(std::string(*it));
		if (endpoint != NULL)
		{
			endpoint->Stop();	
		}
	}

	Node::Postprocess();

}

bool	Device::AddSchedule(std::string const& _id, Timer const& _timer)
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

	for(std::map<std::string, Timer>::iterator it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->second.RemainTime() > _timer.RemainTime())
		{
			endpoint_schedule_list_.insert(it, std::pair<std::string, Timer>(_id, _timer));	
			last = false;
			break;
		}
	}

	if (last)
	{
		endpoint_schedule_list_.insert(endpoint_schedule_list_.end(), std::pair<std::string, Timer>(_id, _timer));	
	}

	endpoint_schedule_list_lock_.unlock();

	return	true;
}

bool	Device::RemoveSchedule(std::string const& _id)
{
	bool	removed = false;
	endpoint_schedule_list_lock_.lock();

	for(std::map<std::string, Timer>::iterator it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
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

bool	Device::IsValidType(std::string const& _type)
{
	return	(std::string(_type) == std::string(DeviceSNMP::Type())) 
			|| (std::string(_type) == std::string(DeviceFTE::Type()))
			|| (std::string(_type) == std::string(DeviceSIM::Type()));
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

bool	Device::WriteValue(std::string const& _endpoint_id, bool _value)
{
	return	false;	
}

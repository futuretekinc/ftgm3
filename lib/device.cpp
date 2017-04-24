#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "trace.h"
#include "device.h"
#include "device_snmp.h"
#include "device_fte.h"
#include "endpoint.h"

static std::list<Device*>	device_list;


Device::Device(Type _type)
:	live_check_interval_(OBJECT_LIVE_CHECK_INTERVAL_SEC * TIME_SECOND)
{
	device_list.push_back(this);
}

Device::~Device()
{
	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		delete *it;
	}
	endpoint_list_lock_.unlock();

	device_list.remove(this);
}

bool	Device::SetLiveCheckInterval(int _interval)
{
	if ((_interval < OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN) || (OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX	< _interval))
	{
		return	false;	
	}

	live_check_interval_ = _interval * TIME_SECOND;
}

bool	Device::SetLiveCheckInterval(Time const& _interval)
{
	if ((uint64_t(_interval) < OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN * TIME_SECOND) || (OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX * TIME_SECOND	< uint64_t(_interval)))
	{
		return	false;	
	}

	live_check_interval_ = _interval;
}

bool	Device::GetProperties(Properties& _properties) const
{
	if (ActiveObject::GetProperties(_properties))
	{
		_properties.Append("type", ToString(GetType()));
		_properties.Append("live_check_interval", live_check_interval_ / TIME_SECOND);

		return	true;	
	}

	return	false;
}

bool	Device::SetProperty(Property const& _property, bool create)
{
	if (_property.GetName() == "live_check_interval")
	{
		
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetLiveCheckInterval(int_value->Get());
		}

		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			uint32_t	value = strtoul(string_value->Get().c_str(), NULL, 10);
			return	SetLiveCheckInterval(value);
		}

		const ValueTime *time_value = dynamic_cast<const ValueTime*>(_property.GetValue());
		if (time_value != NULL)
		{
			return	SetLiveCheckInterval(time_value->Get());
		}
			
		TRACE_INFO << "Property live_check_interval value type is incorrect!" << Trace::End;
	}
	else if (_property.GetName() == "endpoints")
	{
		const ValuePropertiesList* properties_list_value = dynamic_cast<const ValuePropertiesList*>(_property.GetValue());
		if (properties_list_value != NULL)
		{
			const PropertiesList& 	properties_list = properties_list_value->Get();
			for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
			{
				TRACE_INFO << *it << Trace::End;
				if (create)
				{
					Endpoint* endpoint = Endpoint::Create(*it);
					if (endpoint == NULL)
					{
						TRACE_ERROR << "Failed to create endpoint!" << Trace::End;
							return	false;	
					}

					if (!Attach(endpoint))
					{
						delete endpoint;
						return	false;
					}
				}
				else
				{
				}
			}

			return	true;
		}
	}
	else
	{
		return	ActiveObject::SetProperty(_property, create);
	}

	return	false;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Device::Attach(Endpoint* _endpoint)
{
	if (GetEndpoint(_endpoint->GetID()) == NULL)
	{
		endpoint_list_lock_.lock();
		endpoint_list_.push_back(_endpoint);
		endpoint_list_lock_.unlock();

		_endpoint->Attach(this);

		TRACE_INFO << "The endpoint[" << _endpoint->GetID() << "] has been attahced." << Trace::End;
	}

	return	true;
}

bool	Device::Detach(Endpoint* _endpoint)
{
	if (GetEndpoint(_endpoint->GetID()) == NULL)
	{
		TRACE_ERROR << "The endpoint[" << _endpoint->GetID() << "] not attahced." << Trace::End;
		return	false;
	}

	endpoint_list_lock_.lock();
	endpoint_list_.remove(_endpoint);
	endpoint_list_lock_.unlock();

	_endpoint->Detach(this);

	TRACE_INFO << "The endpoint[" << _endpoint->GetID() << "] has been detahced." << Trace::End;

	return	true;	
}

bool	Device::Detach(std::string const& _endpoint_id)
{
	Endpoint* endpoint = GetEndpoint(_endpoint_id);
	if (endpoint == NULL)
	{
		TRACE_ERROR << "The endpoint[" << endpoint->GetID() << "] not attahced." << Trace::End;
		return	false;
	}

	endpoint_list_lock_.lock();
	endpoint_list_.remove(endpoint);
	endpoint_list_lock_.unlock();

	endpoint->Detach(this);

	TRACE_INFO << "The endpoint[" << endpoint->GetID() << "] has been detahced." << Trace::End;

	return	true;	
}

uint32_t	Device::GetEndpointCount()
{
	return	endpoint_list_.size();
}

Endpoint*	Device::GetEndpoint(std::string const& _endpoint_id)
{
	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it)->GetID() == _endpoint_id)
		{
			Endpoint *endpoint = (*it);
			endpoint_list_lock_.unlock();

			return	endpoint;
		}
	}
	endpoint_list_lock_.unlock();

	return	NULL;
}

bool		Device::GetEndpointList(std::list<Endpoint*>& _endpoint_list)
{
	_endpoint_list = endpoint_list_;

	return	true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

Device::operator JSONNode()
{
	Properties	properties;

	GetProperties(properties);

	PropertiesList	properties_list;

	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		Properties	endpoint_properties;
		(*it)->GetProperties(endpoint_properties);
		properties_list.Append(endpoint_properties);
	}

	properties.Append("endpoints", properties_list);

	return	ToJSON(properties);
}

void	Device::Print(std::ostream& os) const
{
	ActiveObject::Print(os);
	os << std::setfill(' ') << std::setw(16) << "Live Check : " << live_check_interval_ / TIME_SECOND << Trace::End;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	ActiveObject::Preprocess();

	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end(); it++)
	{
		(*it)->Start();	
	}
	endpoint_list_lock_.unlock();

	Date date = Date::GetCurrentDate() + live_check_interval_;
	live_check_timer_.Set(date);
}

void	Device::Process()
{
	ValueID id;
	Timer	timer;
	bool	found = false;


	endpoint_list_lock_.lock();

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
	endpoint_list_lock_.unlock();

	if (found)
	{
		Endpoint*	endpoint = GetEndpoint(id);
		if (endpoint != NULL)
		{
			endpoint->UpdateProcess();

			timer += endpoint->GetUpdateInterval();	

			AddSchedule(id, timer);
		}
	}

	if (live_check_timer_.RemainTime() == 0)
	{
		std::cout << "Live check!" << Trace::End;
		live_check_timer_ += live_check_interval_;	
	}
}

void	Device::Postprocess()
{

	endpoint_list_lock_.lock();

	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end(); it++)
	{
		(*it)->Stop();	
	}

	endpoint_list_lock_.unlock();

	ActiveObject::Postprocess();

}

bool	Device::AddSchedule(ValueID const& _id, Timer const& _timer)
{
	bool	last = true;
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

bool	Device::IsValidType(std::string const& _type)
{
	return	((strcasecmp(_type.c_str(), "snmp") == 0) || (strcasecmp(_type.c_str(), "fte") == 0));
}

Device*	Device::Create(Properties const& _properties)
{
	Device*	device = NULL;
	const Property *type_property = _properties.Get("type");

	if (type_property != NULL)
	{
		const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
		if (type_value != NULL)
		{
			Properties	properties(_properties);

			properties.Delete("type");

			if (strcasecmp(type_value->Get().c_str(), "SNMP") == 0)
			{
				device = new DeviceSNMP(properties);
			}
			else if (strcasecmp(type_value->Get().c_str(), "FTE") == 0)
			{
				device = new DeviceFTE(properties);
			}
			else
			{
				TRACE_ERROR2 << "Failed to create device. Device type[" << type_value->Get() << "] is not supported!" << Trace::End;
			}
		}
		else
		{
			TRACE_ERROR2 << "Failed to create device. Device type is invalid!" << Trace::End;
		}
	}
	else
	{
		TRACE_ERROR2 << "Failed to create device. Device type unknown!" << Trace::End;
	}

	return	device;
}

#if 0
Device*	Device::Create(JSONNode const& _node)
{
	if (_node.type() != JSON_NODE)
	{
		return	NULL;
	}

	JSONNode	node = _node;
	JSONNode	endpoint_array(JSON_ARRAY);
	auto it = node.find("endpoints");
	if (it != node.end())
	{
		JSONNode	array = it->as_array();
		for(auto it2 = array.begin(); it2 != array.end() ; it2++)
		{
			endpoint_array.push_back(it2->as_node());
		}

		node.erase(it);
	}

	Properties	properties;	

	properties.Append(node);

	Device* device = Create(properties);
	if (device != NULL)
	{
		for(auto it = endpoint_array.begin(); it != endpoint_array.end() ; it++)
		{
			Endpoint* endpoint = Endpoint::Create(device, it->as_node());		
			if (endpoint == NULL)
			{
				TRACE_ERROR << "Failed to create endpoint!" << Trace::End;	
			}
		}
	}

	TRACE_INFO << "Device[" << device->GetName() << "] created" << Trace::End;

	return	device;
}
#endif

uint32_t	Device::Count()
{
	return	device_list.size();
}

Device*	Device::Get(std::string const& _id)
{
	for(auto it = device_list.begin(); it != device_list.end() ; it++)
	{
		if ((*it)->id_ == _id)
		{
			return	(*it);	
		}
	}

	return	NULL;
}

Device*	Device::Get(uint32_t _index)
{
	for(auto it = device_list.begin(); it != device_list.end() ; it++, _index--)
	{
		if (_index == 0)
		{
			return	(*it);	
		}
	}

	return	NULL;
}

uint32_t	Device::GetList(Type _type, std::list<Device*>& _device_list)
{
	for(auto it = device_list.begin(); it != device_list.end() ; it++)
	{
		if ((*it)->GetType() == _type)
		{
			_device_list.push_back(*it);
		}
	}

	return	_device_list.size();
}

std::string	Device::ToString(Device::Type _type) 
{
	switch(_type)
	{
	case	Device::SNMP:	return	"SNMP";	
	case	Device::FTE:	return	"FTE";	
	case	Device::MBTCP:	return	"MBTCP";	
	}

	return	"Unknown";
}

bool	Device::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back("id");
	_field_list.push_back("name");
	_field_list.push_back("type");
	_field_list.push_back("date");
	_field_list.push_back("enable");
	_field_list.push_back("live_check_interval");
	_field_list.push_back("loop_interval");
	_field_list.push_back("ip");
	_field_list.push_back("module");
	_field_list.push_back("community");
	_field_list.push_back("timeout");

	return	true;
}


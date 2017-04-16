#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "trace.h"
#include "device.h"
#include "device_snmp.h"
#include "endpoint.h"

static std::list<Device*>	device_list;


Device::Device(Type _type)
:	type_(_type), live_check_interval_(OBJECT_LIVE_CHECK_INTERVAL_SEC * TIME_SECOND)
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

Device::Type	Device::GetType()
{
	return	type_;
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

bool	Device::GetProperies(Properties& _properties) const
{
	if (ActiveObject::GetProperties(_properties))
	{
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

		const ValueTime *time_value = dynamic_cast<const ValueTime*>(_property.GetValue());
		if (time_value != NULL)
		{
			return	SetLiveCheckInterval(time_value->Get());
		}
	}
	else if (_property.GetName() == "endpoints")
	{
		const ValuePropertiesList* properties_list_value = dynamic_cast<const ValuePropertiesList*>(_property.GetValue());
		if (properties_list_value != NULL)
		{
			const PropertiesList& 	properties_list = properties_list_value->Get();
			for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
			{
				Endpoint* endpoint = Endpoint::Create(*it);
				if (endpoint != NULL)
				{
					endpoint_list_lock_.lock();
					endpoint_list_.push_back(endpoint);	
					endpoint_list_lock_.unlock();

					endpoint->Attach(this);
				}
				else
				{
					return	false;	
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
	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it) == _endpoint)
		{
			endpoint_list_lock_.unlock();

			return	false;	
		}
	}

	endpoint_list_.push_back(_endpoint);
	endpoint_list_lock_.unlock();

	_endpoint->Attach(this);

	return	true;
}

bool	Device::Detach(Endpoint* _endpoint)
{
	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it) == _endpoint)
		{
			Endpoint *endpoint = (*it);

			endpoint_list_.erase(it);
			endpoint_list_lock_.unlock();

			endpoint->Detach(this);
			return	true;	
		}
	}
	endpoint_list_lock_.unlock();

	return	false;
}

bool	Device::Detach(std::string const& _endpoint_id)
{
	endpoint_list_lock_.lock();
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it)->GetID() == _endpoint_id)
		{
			Endpoint *endpoint = (*it);

			endpoint_list_.erase(it);
			endpoint_list_lock_.unlock();

			endpoint->Detach(this);
			return	true;	
		}
	}
	endpoint_list_lock_.unlock();

	return	false;
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
	os << std::setfill(' ') << std::setw(16) << "Live Check : " << live_check_interval_ / TIME_SECOND << std::endl;
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
	endpoint_schedule_list_lock_.lock();

	auto it = endpoint_schedule_list_.begin();
	if (it != endpoint_schedule_list_.end())
	{
		if (it->second.RemainTime() == 0)
		{
			ValueID id = it->first;
			Timer	timer = it->second;
		
			endpoint_schedule_list_.erase(it);

			Endpoint*	endpoint = GetEndpoint(id);
			if (endpoint != NULL)
			{
				endpoint->UpdateProcess();

				timer += endpoint->GetUpdateInterval();	

				AddSchedule(id, timer);
			}
		}
	}

	endpoint_schedule_list_lock_.unlock();

	if (live_check_timer_.RemainTime() == 0)
	{
		std::cout << "Live check!" << std::endl;
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
	endpoint_schedule_list_lock_.lock();

	for(auto it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->second.RemainTime() > _timer.RemainTime())
		{
			endpoint_schedule_list_.insert(it, std::pair<ValueID, Timer>(_id, _timer));	
			endpoint_schedule_list_lock_.unlock();

			return true;
		}
	}

	endpoint_schedule_list_.push_back(std::pair<ValueID, Timer>(_id, _timer));	
	endpoint_schedule_list_lock_.unlock();

	return	true;
}

bool	Device::RemoveSchedule(ValueID const& _id)
{
	endpoint_schedule_list_lock_.lock();
	for(auto it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->first == _id)
		{
			endpoint_schedule_list_.erase(it);
			endpoint_schedule_list_lock_.unlock();

			return true;
		}
	}
	endpoint_schedule_list_lock_.unlock();

	return	false;
}

bool	Device::IsValidType(std::string const& _type)
{
	if (strcasecmp(_type.c_str(), "snmp") == 0)
	{
		return	true;	
	}
	else if (strcasecmp(_type.c_str(), "fte") == 0)
	{
		return	true;	
	}

	return	false;
}

Device*	Device::Create(Properties const& _properties)
{
	const Property *type_property = _properties.Get("type");
	if (type_property == NULL)
	{
		trace_error << "Failed to create device. Device type unknown!" << std::endl;
		return	NULL;	
	}

	const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
	if (type_value == NULL)
	{
		return	NULL;	
	}

	Properties	properties(_properties);

	properties.Delete("type");

	if (strcasecmp(type_value->Get().c_str(), "SNMP") == 0)
	{
		return	new DeviceSNMP(properties);
	}
}

Device*	Device::Create(JSONNode const& _node)
{
	if (_node.type() != JSON_NODE)
	{
		return	NULL;
	}

	Properties	properties;	

	properties.Append(_node);

	return	Create(properties);
}

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

std::string	ToString(Device::Type _type)
{
	switch(_type)
	{
	case	Device::SNMP:	return	"SNMP";	
	case	Device::MBTCP:	return	"MBTCP";	
	}

	return	"Unknown";
}

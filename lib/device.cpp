#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "trace.h"
#include "device.h"
#include "object_manager.h"
#include "device_snmp.h"
#include "device_fte.h"
#include "device_sim.h"
#include "endpoint.h"


Device::Device(ObjectManager& _manager, ValueType const& _type)
:	ActiveObject(), manager_(_manager), type_(_type), live_check_interval_(OBJECT_LIVE_CHECK_INTERVAL_SEC * TIME_SECOND)
{
	manager_.Attach(this);
}

Device::~Device()
{
	manager_.Detach(this);
}

bool	Device::SetLiveCheckInterval(int _interval, bool _store)
{
	if ((_interval < OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN) || (OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX	< _interval))
	{
		return	false;	
	}

	live_check_interval_ = _interval * TIME_SECOND;

	updated_properties_.AppendLiveCheckInterval(live_check_interval_);
	if (_store)
	{
		ApplyChanges();
	}

	return	true;
}

bool	Device::SetLiveCheckInterval(Time const& _interval, bool _store)
{
	if ((uint64_t(_interval) < OBJECT_LIVE_CHECK_INTERVAL_SEC_MIN * TIME_SECOND) || (OBJECT_LIVE_CHECK_INTERVAL_SEC_MAX * TIME_SECOND	< uint64_t(_interval)))
	{
		return	false;	
	}

	live_check_interval_ = _interval;

	updated_properties_.AppendLiveCheckInterval(live_check_interval_);
	if (_store)
	{
		ApplyChanges();
	}

	return	true;
}

bool	Device::GetProperties(Properties& _properties) const
{
	if (ActiveObject::GetProperties(_properties))
	{
		_properties.AppendDeviceType(GetType());
		_properties.AppendLiveCheckInterval(live_check_interval_ / TIME_SECOND);

		return	true;	
	}

	return	false;
}

bool	Device::SetPropertyInternal(Property const& _property, bool create)
{
	if (create && (_property.GetName() == TITLE_NAME_ID))
	{
		ValueID	old_id = id_;

		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			if (!id_.Set(value->Get()))
			{
				TRACE_ERROR("Failed to set id!");
				return	false;
			}

			TRACE_INFO("The id set to " << id_);

			return	manager_.IDChanged(this, old_id);
		}
		else
		{
			TRACE_INFO("Property id value type is incorrect!");
		}
	}
	else if (_property.GetName() == TITLE_NAME_LIVE_CHECK_INTERVAL)
	{
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetLiveCheckInterval(int_value->Get(), !create);
		}

		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			uint32_t	value = strtoul(string_value->Get().c_str(), NULL, 10);
			return	SetLiveCheckInterval(value, !create);
		}

		const ValueTime *time_value = dynamic_cast<const ValueTime*>(_property.GetValue());
		if (time_value != NULL)
		{
			return	SetLiveCheckInterval(time_value->Get(), !create);
		}
			
		TRACE_INFO("Property live_check_interval value type is incorrect!");
	}
	else if (_property.GetName() == TITLE_NAME_ENDPOINTS)
	{
		const ValuePropertiesList* properties_list_value = dynamic_cast<const ValuePropertiesList*>(_property.GetValue());
		if (properties_list_value != NULL)
		{
			const PropertiesList& 	properties_list = properties_list_value->Get();
			for(auto it = properties_list.begin(); it != properties_list.end() ; it++)
			{
				const Property* device_id_property = it->Get(TITLE_NAME_DEVICE_ID);
				if (device_id_property != NULL)
				{
					TRACE_ERROR("The device id property can't be defined.");
					return	false;
				}

				Properties properties(*it);
				properties.AppendDeviceID(this->GetID());

				TRACE_INFO(properties);
				if (create)
				{
					Endpoint* endpoint = manager_.CreateEndpoint(properties);
					if (endpoint == NULL)
					{
						TRACE_ERROR("Failed to create endpoint!");
							return	false;	
					}

					if (!Attach(endpoint->GetID()))
					{
						TRACE_ERROR("Failed to attach endpoint[" << endpoint->GetTraceName() << "]");
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
		return	ActiveObject::SetPropertyInternal(_property, create);
	}

	return	false;
}


bool	Device::ApplyChanges()
{
	return	manager_.UpdateProperties(this);
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
/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

Device::operator JSONNode()
{
	Properties	properties;

	GetProperties(properties);

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

	properties.Append(Property("endpoints", properties_list));

	return	ToJSON(properties);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	ActiveObject::Preprocess();

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

	Date date = Date::GetCurrentDate() + live_check_interval_;
	live_check_timer_.Set(date);
	TRACE_INFO("live checker start [" << date << "]");
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
			endpoint->UpdateProcess();

			timer += endpoint->GetUpdateInterval();	

			AddSchedule(id, timer);
		}
	}

	if (live_check_timer_.RemainTime() == 0)
	{
		Message*	message = new MessageKeepAlive(id_);

		TRACE_INFO("Post live check!");	
		if (!manager_.Post(message))
		{
			TRACE_ERROR("Failed to post mesage to manager!");	

			delete message;
		}

		live_check_timer_ += live_check_interval_;	
	}
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

	ActiveObject::Postprocess();

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

bool	Device::IsValidType(std::string const& _type)
{
	return	(std::string(_type) == std::string(DeviceSNMP::Type())) || (std::string(_type) == std::string(DeviceFTE::Type()));
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

			else if (std::string(*type_value) == std::string(DeviceFTE::Type()))
			{
				device = new DeviceFTE(_manager, properties);
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
#endif


const	ValueType&	Device::Type()
{
	static	ValueType	type_("device");

	return	type_;
}

bool	Device::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(TITLE_NAME_ID);
	_field_list.push_back(TITLE_NAME_NAME);
	_field_list.push_back(TITLE_NAME_TYPE);
	_field_list.push_back(TITLE_NAME_DATE);
	_field_list.push_back(TITLE_NAME_ENABLE);
	_field_list.push_back(TITLE_NAME_DEVICE_ID);
	_field_list.push_back(TITLE_NAME_LIVE_CHECK_INTERVAL);
	_field_list.push_back(TITLE_NAME_LOOP_INTERVAL);
	_field_list.push_back(TITLE_NAME_IP);
	_field_list.push_back(TITLE_NAME_MODULE);
	_field_list.push_back(TITLE_NAME_COMMUNITY);
	_field_list.push_back(TITLE_NAME_TIMEOUT);
	return	true;
}


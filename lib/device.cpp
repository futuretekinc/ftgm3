#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "device.h"
#include "device_snmp.h"
#include "endpoint.h"

static std::list<Device*>	device_list;


Device::Device()
:	live_check_interval_(OBJECT_LIVE_CHECK_INTERVAL_SEC * TIME_SECOND)
{
	device_list.push_back(this);
}

Device::~Device()
{
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

bool	Device::GetProperies(Properties& _properties)
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

	return	ActiveObject::SetProperty(_property, create);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Device::Attach(Endpoint* _endpoint)
{
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it) == _endpoint)
		{
			return	false;	
		}
	}

	endpoint_list_.push_back(_endpoint);
	_endpoint->Attach(this);

	return	true;
}

bool	Device::Detach(Endpoint* _endpoint)
{
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it) == _endpoint)
		{
			_endpoint->Detach(this);
			endpoint_list_.erase(it);
			return	true;	
		}
	}

	return	false;
}

bool	Device::Detach(std::string const& _endpoint_id)
{
	for(auto it = endpoint_list_.begin(); it != endpoint_list_.end() ; it++)
	{
		if ((*it)->GetID() == _endpoint_id)
		{
			(*it)->Detach(this);
			endpoint_list_.erase(it);
			return	true;	
		}
	}

	return	false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

void	Device::Print(std::ostream& os) const
{
	ActiveObject::Print(os);
	os << std::setw(16) << "Live Check : " << live_check_interval_ / TIME_SECOND << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	Date	date;

	date = Date::GetCurrentDate() + live_check_interval_;

	live_check_timer_.Set(date);
}

void	Device::Process()
{
	if (live_check_timer_.RemainTime() == 0)
	{
		live_check_timer_ += live_check_interval_;	
	}
}

void	Device::Postprocess()
{
}


Device*	Device::Create(Properties const& _properties)
{
	const Property *type_property = _properties.Get("type");
	if (type_property == NULL)
	{
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

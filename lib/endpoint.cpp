#include <list>
#include "defined.h"
#include "device.h"
#include "endpoint.h"
#include "endpoint_sensor_temperature.h"

using namespace std;

static list<Endpoint*>	endpoint_list;

Endpoint::ValueMap::ValueMap(uint32_t _limit)
:	limit_(_limit)
{
}

Endpoint::ValueMap::~ValueMap()
{
	ValueMap::iterator it = begin();
	while(it != end())
	{
		delete it->second;
	}
}

bool Endpoint::ValueMap::Add(Date _date, Value const *_value)
{
	if (_date.IsValid() && (_value != NULL) && (size() < limit_))
	{
		insert(std::make_pair(_date, _value->Duplicate()));
		return	true;
	}

	return	false;
}

Endpoint::Endpoint()
:	active_(false), update_interval_(ENDPOINT_UPDATE_INTERVAL), unit_(""), scale_(ENDPOINT_VALUE_SCALE), value_(0), value_map_(ENDPOINT_VALUE_COUNT_MAX), device_(NULL)
{
	endpoint_list.push_back(this);
}

Endpoint::Endpoint(string const& _unit)
:	active_(false), update_interval_(ENDPOINT_UPDATE_INTERVAL), unit_(_unit), scale_(ENDPOINT_VALUE_SCALE), value_(0), value_map_(ENDPOINT_VALUE_COUNT_MAX), device_(NULL)
{
	endpoint_list.push_back(this);
}

Endpoint::~Endpoint()
{
	endpoint_list.remove(this);
}

const	ValueUnit&	Endpoint::GetUnit() const
{
	return	unit_;
}

bool	Endpoint::SetUnit(std::string const& _unit)
{
	unit_ = _unit;

	return	true;
}

float	Endpoint::GetScale() const
{
	return	scale_;
}

bool	Endpoint::SetScale(float _scale)
{
	scale_ = _scale;
}

bool	Endpoint::SetUpdateInterval(Time const& _update_interval)
{
	update_interval_ = _update_interval;
}

bool	Endpoint::SetUpdateInterval(uint64_t _update_interval)
{
	update_interval_ = _update_interval;
}

uint64_t	Endpoint::GetUpdateInterval()
{
	return	update_interval_;
}

bool	Endpoint::GetProperies(Properties& _properties)
{
	if (ActiveObject::GetProperties(_properties))
	{
		_properties.Append("unit", unit_);
		_properties.Append("scale", float(scale_));
		_properties.Append("update_interval", update_interval_);

		return	true;	
	}

	return	false;
}

bool	Endpoint::SetProperty(Property const& _property, bool create)
{
	if (_property.GetName() == "unit")
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			return	SetUnit(string_value->Get());
		}
	}
	else if (_property.GetName() == "scale")
	{
		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetScale(float_value->Get());
		}

		const ValueInt *int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetScale(int_value->Get());
		}
	}
	else if (_property.GetName() == "update_interval")
	{
		const ValueTime *time_value = dynamic_cast<const ValueTime*>(_property.GetValue());
		if (time_value != NULL)
		{
			return	SetUpdateInterval(time_value->Get());
		}
		
		const ValueInt *int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (time_value != NULL)
		{
			return	SetUpdateInterval(time_value->Get());
		}
	}

	return	ActiveObject::SetProperty(_property, create);
}

bool	Endpoint::Attach(Device* _device)
{
	if (device_ != NULL)
	{
		if (device_ != _device)
		{
			device_->Detach(this);
		}
	}
	else
	{
		device_ = _device;
		device_->Attach(this);
	}

	return	true;
}

bool	Endpoint::Detach(Device* _device)
{
	if (device_ == _device)
	{
		Stop();

		Device*	device = device_;

		device_ = NULL;

		device->Detach(this);
		return	true;
	}

	return	false;
}

void	Endpoint::Start()
{
	if (active_)
	{
		ActiveObject::Start();
	}
	else
	{
		if (device_ != NULL)
		{
			Timer	timer;

			device_->AddSchedule(id_, timer);
		}
	}
}

void	Endpoint::Stop()
{
	if (active_)
	{
		ActiveObject::Stop();
	}
	else
	{
		if (device_ != NULL)
		{
			device_->RemoveSchedule(id_);
		}
	}
}

void	Endpoint::Preprocess()
{
	Date	date;

	date = Date::GetCurrentDate() + update_interval_;

	update_timer_.Set(date);
}

void	Endpoint::Process()
{
	if (update_timer_.RemainTime() == 0)
	{
		UpdateProcess();
		update_timer_ += update_interval_;	
	}
}

void	Endpoint::Postprocess()
{
}

void	Endpoint::UpdateProcess()
{
}

uint32_t	Endpoint::GetValueCount()
{
	return	value_map_.size();
}

Date	Endpoint::GetFirstDate()
{
	ValueMap::iterator it = value_map_.begin();
	if (it == value_map_.end())
	{
		return	Date(0);	
	}
	else
	{
		return	it->first;	
	}
}

Date	Endpoint::GetLastDate()
{
	ValueMap::reverse_iterator it = value_map_.rbegin();
	if (it == value_map_.rend())
	{
		return	Date(0);	
	}
	else
	{
		return	it->first;	
	}
}

bool	Endpoint::GetPeriodValues(Date const& _begin, Date const& _end, ValueMap& _value_map)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		_value_map.Add(it->first, it->second);
	}
}

bool	Endpoint::RemovePeriodValues(Date const& _begin, Date const& _end)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		value_map_.erase(it);
	}
}

bool	Endpoint::Add(Date _date, Value const *_value)
{
	return	value_map_.Add(_date, _value);
}

Endpoint*	Endpoint::Create(Properties const& _properties)
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

	if (strcasecmp(type_value->Get().c_str(), "sensor_temperature") == 0)
	{
		return	new EndpointSensorTemperature(properties);
	}
}

Endpoint*	Endpoint::Get(std::string const& _id)
{
	for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
	{
		if ((*it)->id_ == _id)
		{
			return	(*it);	
		}
	}

	return	NULL;
}

uint32_t	Endpoint::Count()
{
	return	endpoint_list.size();
}

uint32_t	Endpoint::UnattachedCount()
{
	uint32_t	count = 0;

	for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
	{
		if ((*it)->device_ == 0)
		{
			count++;
		}
	}

	return	count;
}

#include <list>
#include "defined.h"
#include "endpoint.h"

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
:	update_interval_(ENDPOINT_UPDATE_INTERVAL), unit_(""), scale_(ENDPOINT_VALUE_SCALE), value_(0), value_map_(ENDPOINT_VALUE_COUNT_MAX)
{
	endpoint_list.push_back(this);
}

Endpoint::Endpoint(string const& _unit)
:	update_interval_(ENDPOINT_UPDATE_INTERVAL), unit_(_unit), scale_(ENDPOINT_VALUE_SCALE), value_(0), value_map_(ENDPOINT_VALUE_COUNT_MAX)
{
	endpoint_list.push_back(this);
}

Endpoint::~Endpoint()
{
	endpoint_list.remove(this);
}

bool	Endpoint::SetUnit(std::string const& _unit)
{
	unit_ = _unit;

	return	true;
}

bool	Endpoint::SetScale(float _scale)
{
	scale_ = _scale;
}

bool	Endpoint::SetUpdateInterval(Time const& _update_interval)
{
	update_interval_ = _update_interval;
}

bool	Endpoint::SetUpdateInterval(int _update_interval)
{
	update_interval_ = _update_interval;
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

void	Endpoint::Preprocess()
{
	Date	date;

	date = Date::GetCurrentDate() + update_interval_;

	update_timer_.Set(date);
}

bool	Endpoint::Attach(Device* _device)
{
	device_ = _device;

	return	true;
}

bool	Endpoint::Detach(Device* _device)
{
	if (device_ == _device)
	{
		device_ = NULL;
		return	true;
	}

	return	false;
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

bool	Endpoint::Add(Date _date, Value const *_value)
{
	return	value_map_.Add(_date, _value);
}

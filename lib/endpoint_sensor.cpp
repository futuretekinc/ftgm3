#include "device.h"
#include "endpoint_sensor.h"

EndpointSensor::EndpointSensor(ObjectManager& _manager)
:Endpoint(_manager)
{
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _value_max)
: Endpoint(_manager)
{
	unit_ = _unit;
	if (_value_min < _value_max)
	{
		value_min_ = _value_min;	
		value_max_ = _value_max;	
	}
	else
	{
		value_min_ = _value_max;	
		value_max_ = _value_min;	
	}
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, std::string const& _unit, float _value_min, float _value_max)
: Endpoint(_manager)
{
	unit_ = _unit;
	if (_value_min < _value_max)
	{
		value_min_ = _value_min;	
		value_max_ = _value_max;	
	}
	else
	{
		value_min_ = _value_max;	
		value_max_ = _value_min;	
	}
}

bool	EndpointSensor::IsValid(const ValueFloat& _value)
{
	return	((value_min_ <= _value) && (_value <= value_max_));
}

bool	EndpointSensor::SetValueMin(ValueFloat const& _value_min, bool _store)
{
	value_min_ = _value_min;
	
	updated_properties_.AppendValueMin(value_min_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

bool	EndpointSensor::SetValueMax(ValueFloat const& _value_max, bool _store)
{
	value_max_ = _value_max;

	updated_properties_.AppendValueMax(value_max_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

bool	EndpointSensor::SetPropertyInternal(Property const& _property, bool create)
{
	if (_property.GetName() == "min")
	{
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetValueMin(int_value->Get(), !create);
		}

		const ValueFloat*	float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetValueMin(float_value->Get(), !create);
		}
	}
	else if (_property.GetName() == "max")
	{
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetValueMin(int_value->Get(), !create);
		}

		const ValueFloat*	float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetValueMax(float_value->Get(), !create);
		}
	}
	else
	{
		return	Endpoint::SetPropertyInternal(_property, create);	
	}

	return	false;
}


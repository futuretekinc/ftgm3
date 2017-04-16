#include "endpoint_sensor.h"

EndpointSensor::EndpointSensor()
:Endpoint()
{
}

EndpointSensor::EndpointSensor(std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _value_max)
: Endpoint(_unit)
{
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

EndpointSensor::EndpointSensor(std::string const& _uint, float _value_min, float _value_max)
: Endpoint(_uint)
{
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

bool	EndpointSensor::SetValueMin(ValueFloat const& _value_min)
{
	value_min_ = _value_min;

	return	true;
}

bool	EndpointSensor::SetValueMax(ValueFloat const& _value_max)
{
	value_max_ = _value_max;

	return	true;
}

bool	EndpointSensor::SetProperty(Property const& _property)
{
	if (_property.GetName() == "min")
	{
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetValueMin(int_value->Get());
		}

		const ValueFloat*	float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetValueMin(float_value->Get());
		}
	}
	else if (_property.GetName() == "max")
	{
		const ValueInt*	int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (int_value != NULL)
		{
			return	SetValueMin(int_value->Get());
		}

		const ValueFloat*	float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			return	SetValueMax(float_value->Get());
		}
	}
	else
	{
		return	Endpoint::SetProperty(_property);	
	}

	return	false;
}

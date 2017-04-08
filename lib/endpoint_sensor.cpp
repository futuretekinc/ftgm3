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

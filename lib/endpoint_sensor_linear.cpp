#include "defined.h"
#include "utils.h"
#include "device.h"
#include "endpoint_sensor_linear.h"

static const char* class_name = "EPSLinuear";

EndpointSensorLinear::EndpointSensorLinear(ObjectManager& _manager, std::string const& _type, std::string const& _unit, double _min, double _max)
: EndpointSensor(_manager, _type, _unit), value_(0), value_min_(_min), value_max_(_max)
{
	trace.SetClassName(class_name);
}

EndpointSensorLinear::EndpointSensorLinear(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties, std::string const& _unit, double _min, double _max)
: EndpointSensor(_manager, _type, _unit),value_(0), value_min_(_min), value_max_(_max)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char*	EndpointSensorLinear::GetClassName() 
{	
	return	class_name;	
}

bool	EndpointSensorLinear::IsValid(std::string const& _value)
{
	double	value = strtod(_value.c_str(), NULL);

	return	((value_min_ <= value) && (value <= value_max_));
}

std::string EndpointSensorLinear::GetValue()
{
	return	ToString(value_, 2);
}

bool		EndpointSensorLinear::SetValue(std::string const& _value, bool _check)
{
	if (!_check)
	{
		value_ = strtod(_value.c_str(),NULL);
	}

	return	true;
}

std::string EndpointSensorLinear::GetValueMin()
{
	return	ToString(value_min_, 2);
}

bool		EndpointSensorLinear::SetValueMin(std::string const& _value, bool _check)
{
	if (!_check)
	{
		value_min_ = strtod(_value.c_str(), NULL);
	}

	return	true;
}

std::string EndpointSensorLinear::GetValueMax()
{
	return	ToString(value_max_, 2);
}

bool		EndpointSensorLinear::SetValueMax(std::string const& _value, bool _check)
{
	if (!_check)
	{
		value_max_ = strtod(_value.c_str(), NULL);
	}

	return	true;
}

bool	EndpointSensorLinear::GetProperty(uint32_t _type, JSONNode& _property)
{
	switch(_type)
	{
	case	PROPERTY_VALUE_MIN_FLAG:	_property = JSONNode(TITLE_NAME_VALUE_MIN, GetValueMin()); break;
	case	PROPERTY_VALUE_MAX_FLAG:	_property = JSONNode(TITLE_NAME_VALUE_MAX, GetValueMax()); break;
	default:
		return	EndpointSensor::GetProperty(_type, _property);
	}

	return	true;
}

bool	EndpointSensorLinear::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_VALUE_MIN)
	{
		ret_value = SetValueMin(_property.as_string(), _check);
	}
	else if (_property.name() == TITLE_NAME_VALUE_MAX)
	{
		ret_value = SetValueMax(_property.as_string(), _check);
	}
	else
	{
		ret_value = EndpointSensor::SetProperty(_property, _check);	
	}

	return	ret_value;
}

bool	EndpointSensorLinear::Add(time_t time, std::string const& _value)
{
	if (time_ != time)
	{
		time_ = time;

		value_= strtod(_value.c_str(), 0); 

		return	Endpoint::Add(time, _value);
	}

	return	true;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorLinear::Type()
{
	return	OBJECT_TYPE_EP_S_LINEAR;
}


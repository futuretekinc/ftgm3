#include "device.h"
#include "defined.h"
#include "object_manager.h"
#include "endpoint_sensor.h"

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type)
:	Endpoint(_manager, _type),
	value_(0), value_min_(0), value_max_(100)
{
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _value_max)
: Endpoint(_manager, _type, _unit)
{
	value_ = 0;
	value_min_ = _value_min.Get();
	value_max_ = _value_max.Get();
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _unit, float _value_min, float _value_max)
: Endpoint(_manager, _type, _unit), value_min_(_value_min), value_max_(_value_max)
{
}

bool	EndpointSensor::IsValid(const ValueFloat& _value)
{
	return	((value_min_ <= _value) && (_value <= value_max_));
}

std::string	EndpointSensor::GetValue()
{
	return	std::to_string(value_);
}

bool		EndpointSensor::SetValue(std::string const& _value)
{
	value_ = strtod(_value.c_str(),NULL);

	return	true;
}

std::string	EndpointSensor::GetValueMin()
{
	return	std::to_string(value_);
}

bool		EndpointSensor::SetValueMin(std::string const& _value)
{
	value_min_ = strtod(_value.c_str(), NULL);

	return	true;
}

std::string	EndpointSensor::GetValueMax()
{
	return	std::to_string(value_max_);
}

bool		EndpointSensor::SetValueMax(std::string const& _value)
{
	value_max_ = strtod(_value.c_str(), NULL);

	return	true;
}

bool	EndpointSensor::GetProperties(Properties& _properties, Properties::Fields const& _fields)
{
	return	Endpoint::GetProperties(_properties, _fields);
}

bool	EndpointSensor::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	if (_property.GetName() == TITLE_NAME_VALUE_MIN)
	{
		const Value*	value = _property.GetValue();

		return	SetValueMin(std::string(*value));
	}
	else if (_property.GetName() == TITLE_NAME_VALUE_MAX)
	{
		const Value*	value = _property.GetValue();

		return	SetValueMax(std::string(*value));
	}
	else
	{
		return	Endpoint::SetProperty(_property, _fields);	
	}

	return	false;
}

void	EndpointSensor::Preprocess()
{
	Date	date;

	date = Date::GetCurrent() + Time(correction_interval_ * TIME_SECOND);

	correction_timer_.Set(date);

	Endpoint::Preprocess();
}

void	EndpointSensor::Process()
{
	if (correction_timer_.RemainTime() == 0)
	{
		CorrectionProcess();
		correction_timer_ += Time(correction_interval_ * TIME_SECOND);	
	}

	Endpoint::Process();
}

void	EndpointSensor::Postprocess()
{
	Endpoint::Postprocess();
}

bool	EndpointSensor::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<EndpointSensor *>(_object) != NULL;
}

#include "defined.h"
#include "exception.h"
#include "device.h"
#include "utils.h"
#include "endpoint_sensor_discrete.h"

EndpointSensorDiscrete::EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type)
: EndpointSensor(_manager, _type)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorDiscrete::EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties)
: EndpointSensor(_manager, _type)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorDiscrete::GetClassName() 
{	
	return	"EPSDiscrete";	
}

const 	std::string 	EndpointSensorDiscrete::Type()
{
	return	std::string(NODE_TYPE_EP_S_DISCRETE);
}

bool	EndpointSensorDiscrete::IsValid(std::string const& _value)
{
	try
	{
		bool	value = IsTrue(_value);

		return	true;
	}
	catch(InvalidArgument& e)
	{
		return	false;
	}
}

std::string	EndpointSensorDiscrete::GetValue()
{
	return	::ToString(value_);
}

bool		EndpointSensorDiscrete::SetValue(std::string const& _value, bool _check)
{
	if (!_check)
	{
		if (IsValid(_value))
		{
			if (IsTrue(_value))
			{
				value_ = true;
			}
			else
			{
				value_ = false;
			}
		}
		else
		{
			return	false;	
		}
	}

	return	true;
}

std::string	EndpointSensorDiscrete::GetValueMin()
{
	return	::ToString(false);
}

bool		EndpointSensorDiscrete::SetValueMin(std::string const& _value, bool _check)
{
	return	true;
}

std::string	EndpointSensorDiscrete::GetValueMax()
{
	return	::ToString(true);
}

bool		EndpointSensorDiscrete::SetValueMax(std::string const& _value, bool _check)
{
	return	true;
}

bool	EndpointSensorDiscrete::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	return	EndpointSensor::GetProperties(_properties, _fields);
}

bool	EndpointSensorDiscrete::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_VALUE_MIN)
	{
	}
	else if (_property.name() == TITLE_NAME_VALUE_MAX)
	{
	}
	else
	{
		ret_value = EndpointSensor::SetProperty(_property, _check);	
	}

	return	ret_value;
}


#include "defined.h"
#include "exception.h"
#include "device.h"
#include "utils.h"
#include "endpoint_sensor_discrete.h"

static const char*	class_name = "EPSDiscrete";

EndpointSensorDiscrete::EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type)
: EndpointSensor(_manager, _type), value_(false)
{
	trace.SetClassName(class_name);
}

EndpointSensorDiscrete::EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties)
: EndpointSensor(_manager, _type), value_(false)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char* EndpointSensorDiscrete::GetClassName() 
{	
	return	class_name;
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

			TRACE_INFO("DI_DATA 01 :" << value_);
		}
		else
		{

			TRACE_INFO("DI_DATA 02 :" << value_);
			return	false;	
		}
	}
	TRACE_INFO("DI_CHECK_ERROR");

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

bool	EndpointSensorDiscrete::Add(time_t time, std::string const& _value)
{
	if(time_ != time)
	{
		time_ = time;

		SetValue(_value, false);

		return Endpoint::Add(time, _value);
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorDiscrete::Type()
{
	return	OBJECT_TYPE_EP_S_DISCRETE;
}


#include "defined.h"
#include "exception.h"
#include "json.h"
#include "object_manager.h"
#include "device.h"
#include "utils.h"
#include "endpoint_actuator_discrete.h"

EndpointActuatorDiscrete::EndpointActuatorDiscrete(ObjectManager& _manager, std::string const& _type)
: EndpointActuator(_manager, _type), value_(false)
{
	trace.SetClassName(GetClassName());
}

EndpointActuatorDiscrete::EndpointActuatorDiscrete(ObjectManager& _manager, std::string const& _type,  JSONNode const& _properties)
: EndpointActuator(_manager, _type),value_(false)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointActuatorDiscrete::GetClassName()
{
	return	"EPSDiscrete";
}

const std::string EndpointActuatorDiscrete::Type()
{
	return	std::string(NODE_TYPE_EP_A_DISCRETE);
}

bool	EndpointActuatorDiscrete::IsValid(std::string const& _value)
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

std::string	EndpointActuatorDiscrete::GetValue()
{
	return	ToString(value_);
}

bool	EndpointActuatorDiscrete::SetValue(std::string const& _value, bool _check)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	if (!_check)
	{
		bool	value = IsTrue(_value);

		if (value_ != value)
		{
			Device*	device = manager_.GetDevice(parent_id_);
			if (device == NULL)
			{
				TRACE_ERROR("Device not found!");
				return	false;
			}

			if (!device->WriteValue(id_, value))
			{
				TRACE_ERROR("Failed to write value to device!");
				return	false;
			}

			time_t	time;

			device->ReadValue(id_, time, value_);

			Add(time, value_);
		}
	}
	return	true;
}

std::string	EndpointActuatorDiscrete::GetValueMin()
{
	return	ToString(false);
}

bool	EndpointActuatorDiscrete::SetValueMin(std::string const& _value, bool _check)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	return	true;
}

std::string	EndpointActuatorDiscrete::GetValueMax()
{
	return	ToString(true);
}

bool	EndpointActuatorDiscrete::SetValueMax(std::string const& _value, bool _check)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	return	true;
}



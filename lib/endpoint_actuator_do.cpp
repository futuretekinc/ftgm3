#include "defined.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint_actuator_do.h"

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager)
: EndpointActuator(_manager, EndpointActuatorDO::Type()), value_(ENDPOINT_ACTUATOR_DO_OFF)
{
	trace.SetClassName(GetClassName());
}

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager, Properties const& _properties)
: EndpointActuator(_manager, EndpointActuatorDO::Type()),value_(ENDPOINT_ACTUATOR_DO_OFF)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , PROPERTY_ALL, true);
}

std::string	EndpointActuatorDO::GetClassName()
{
	return	"EPSDO";
}

const ValueType EndpointActuatorDO::Type()
{
	return	ValueType(NODE_TYPE_EP_A_DO);
}

bool		EndpointActuatorDO::IsValid(std::string const& _value)
{
	return	(_value == ENDPOINT_ACTUATOR_DO_ON) || (_value == ENDPOINT_ACTUATOR_DO_OFF);
}

std::string	EndpointActuatorDO::GetValue()
{
	return	value_;
}

bool	EndpointActuatorDO::SetValue(std::string const& _value)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	if (value_ != _value)
	{
		Device*	device = manager_.GetDevice(parent_id_);
		if (device == NULL)
		{
			TRACE_ERROR("Device not found!");
			return	false;
		}

		if (!device->WriteValue(id_, _value))
		{
			TRACE_ERROR("Failed to write value to device!");
			return	false;
		}

		device->ReadValue(id_, value_);

		Add(value_);
	}

	return	true;
}

std::string	EndpointActuatorDO::GetValueMin()
{
	return	value_min_;
}

bool	EndpointActuatorDO::SetValueMin(std::string const& _value)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	if (value_min_ != _value)
	{
		value_min_ = _value;

		updated_properties_.AppendValueMin(value_min_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

std::string	EndpointActuatorDO::GetValueMax()
{
	return	value_max_;
}

bool	EndpointActuatorDO::SetValueMax(std::string const& _value)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	if (value_max_ != _value)
	{
		value_max_ = _value;

		updated_properties_.AppendValueMax(value_max_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}
	return	true;
}



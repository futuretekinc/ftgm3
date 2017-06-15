#include "device.h"
#include "defined.h"
#include "object_manager.h"
#include "endpoint_sensor.h"

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type)
:	Endpoint(_manager, _type),
	correction_interval_(ENDPOINT_UPDATE_INTERVAL)
{
}

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _unit, const ValueFloat& _value_min, const ValueFloat& _value_max)
: Endpoint(_manager, _type)
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

EndpointSensor::EndpointSensor(ObjectManager& _manager, ValueType const& _type, std::string const& _unit, float _value_min, float _value_max)
: Endpoint(_manager, _type)
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

bool	EndpointSensor::SetCorrectionInterval(Time const& _interval)
{
	correction_interval_ = _interval;

	updated_properties_.AppendCorrectionInterval(correction_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	EndpointSensor::SetCorrectionInterval(uint32_t _interval)
{
	correction_interval_ = _interval;

	updated_properties_.AppendCorrectionInterval(correction_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	EndpointSensor::SetCorrectionInterval(std::string const& _interval)
{
	correction_interval_ = strtoul(_interval.c_str(), NULL, 10);
	
	updated_properties_.AppendCorrectionInterval(correction_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

uint32_t	EndpointSensor::GetCorrectionInterval()
{
	return	correction_interval_;
}

bool	EndpointSensor::SetValueMin(ValueFloat const& _value_min)
{
	value_min_ = _value_min;
	
	updated_properties_.AppendValueMin(value_min_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	EndpointSensor::SetValueMax(ValueFloat const& _value_max)
{
	value_max_ = _value_max;

	updated_properties_.AppendValueMax(value_max_);
	
	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	EndpointSensor::GetProperties(Properties& _properties, Properties::Fields const& _fields)
{
	if (Endpoint::GetProperties(_properties, _fields))
	{	
		if (_fields.correction_interval)
		{
			_properties.AppendCorrectionInterval(correction_interval_);
		}

		return	true;	
	}

	return	false;
}

bool	EndpointSensor::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	if (_property.GetName() == TITLE_NAME_CORRECTION_INTERVAL)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The correction interval set to " << string_value->Get());
			return	SetCorrectionInterval(string_value->Get());
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The correction interval set to " << float_value->Get());
			return	SetCorrectionInterval(float_value->Get());
		}

		const ValueUInt32 *int_value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (int_value != NULL)
		{
			TRACE_INFO("The correction interval set to " << int_value->Get());
			return	SetCorrectionInterval(int_value->Get());
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_VALUE_MIN)
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
	else if (_property.GetName() == TITLE_NAME_VALUE_MAX)
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

void	EndpointSensor::CorrectionProcess()
{
	ValueFloat	value;

	Device* device = manager_.GetDevice(parent_id_);
	if(device != NULL)
	{
		if (device->ReadValue(GetID(), &value))
		{
			if (IsValid(value))
			{
				if (!Add(&value))
				{
					TRACE_ERROR("Failed to add data");	
				}
			}
			else
			{
				TRACE_ERROR("Data is invalid!");
			}
		}
		else
		{
			TRACE_ERROR("Failed to read data from device[" << device->GetTraceName() << "]");	
		}
	}
}


#include <list>
#include "defined.h"
#include "device.h"
#include "trace.h"
#include "value.h"
#include "endpoint.h"
#include "object_manager.h"
#include "endpoint_sensor_temperature.h"
#include "endpoint_sensor_humidity.h"

Endpoint::Endpoint(ObjectManager& _manager)
:	ActiveObject(),
	manager_(_manager), 
	device_id_(""), 
	active_(false), 
	update_interval_(ENDPOINT_UPDATE_INTERVAL), 
	unit_(""), 
	scale_(ENDPOINT_VALUE_SCALE), 
	value_(0), 
	value_map_(ENDPOINT_VALUE_COUNT_MAX),
	last_report_date_()
{
	manager_.Attach(this);
}

Endpoint::~Endpoint()
{
	manager_.Detach(this);
}

const	ValueUnit&	Endpoint::GetUnit() const
{
	return	unit_;
}

bool	Endpoint::SetUnit(std::string const& _unit, bool _store)
{
	unit_ = _unit;
	
	updated_properties_.AppendUnit(unit_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

float	Endpoint::GetScale() const
{
	return	scale_;
}

bool	Endpoint::SetScale(std::string const& _scale, bool _store)
{
	scale_ = strtod(_scale.c_str(), NULL);

	updated_properties_.AppendScale(scale_);
	if (_store)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::SetScale(float _scale, bool _store)
{
	scale_ = _scale;

	updated_properties_.AppendScale(scale_);
	if (_store)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::SetUpdateInterval(Time const& _update_interval, bool _store)
{
	update_interval_ = _update_interval;

	updated_properties_.AppendUpdateInterval(update_interval_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

bool	Endpoint::SetUpdateInterval(uint64_t _update_interval, bool _store)
{
	update_interval_ = _update_interval;

	updated_properties_.AppendUpdateInterval(update_interval_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

bool	Endpoint::SetUpdateInterval(std::string const& _update_interval, bool _store)
{
	update_interval_ = strtoul(_update_interval.c_str(), NULL, 10);
	
	updated_properties_.AppendUpdateInterval(update_interval_);
	if (_store)
	{
		ApplyChanges();	
	}
	return	true;
}

uint64_t	Endpoint::GetUpdateInterval()
{
	return	update_interval_;
}

std::string	Endpoint::GetSensorID() const
{
	return	sensor_id_;
}

bool	Endpoint::SetSensorID(std::string const& _sensor_id, bool _store)
{
	sensor_id_ = _sensor_id;

	updated_properties_.AppendSensorID(sensor_id_);
	if (_store)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::GetProperties(Properties& _properties) const
{
	if (ActiveObject::GetProperties(_properties))
	{	
		_properties.AppendEndpointType(Endpoint::ToString(GetType()));
		_properties.AppendSensorID(sensor_id_);
		_properties.AppendDeviceID(std::string(device_id_));
		_properties.AppendUnit(unit_);
		_properties.AppendScale(float(scale_));
		_properties.AppendUpdateInterval(update_interval_);

		return	true;	
	}

	return	false;
}

bool	Endpoint::SetPropertyInternal(Property const& _property, bool create)
{
	if (create && (_property.GetName() == TITLE_NAME_ID))
	{
		ValueID	old_id = id_;

		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			if (!id_.Set(value->Get()))
			{
				TRACE_ERROR("Failed to set id!");
				return	false;
			}

			TRACE_INFO("The id set to " << id_);

			return	manager_.IDChanged(this, old_id);
		}
		else
		{
			TRACE_INFO("Property id value type is incorrect!");
		}
	}
	else if (_property.GetName() == TITLE_NAME_UNIT)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The unit set to " << string_value->Get());
			return	SetUnit(string_value->Get(), !create);
		}

		TRACE_ERROR("Property[" << id_ << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_SCALE)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The scale set to " << string_value->Get());
			return	SetScale(string_value->Get(), !create);
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The scale set to " << float_value->Get());
			return	SetScale(float_value->Get(), !create);
		}

		const ValueInt *int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The scale set to " << int_value->Get());
			return	SetScale(int_value->Get(), !create);
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_UPDATE_INTERVAL)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The update interval set to " << string_value->Get());
			return	SetUpdateInterval(string_value->Get(), !create);
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The update interval set to " << float_value->Get());
			return	SetUpdateInterval(float_value->Get(), !create);
		}

		const ValueUInt32 *int_value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (int_value != NULL)
		{
			TRACE_INFO("The update interval set to " << int_value->Get());
			return	SetUpdateInterval(int_value->Get(), !create);
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_SENSOR_ID)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			TRACE_INFO("The sensor id set to " << value->Get());
			return	SetSensorID(value->Get(), !create);
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_DEVICE_ID)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			return	SetDeviceID(value->Get(), !create);
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}

	return	ActiveObject::SetPropertyInternal(_property, create);
}

bool	Endpoint::ApplyChanges()
{
	return	manager_.UpdateProperties(this);
}

const ValueID&	Endpoint::GetDeviceID() const
{
	return	device_id_;
}

bool	Endpoint::SetDeviceID(ValueID const& _device_id, bool _store) 
{
	ValueID old_id(device_id_);

	if (!device_id_.Set(_device_id))
	{
		TRACE_ERROR("The device id is invalid["<< _device_id << "]");
		return	false;	
	}

	updated_properties_.AppendDeviceID(_device_id);
	if (_store)
	{
		ApplyChanges();	
	}
	TRACE_INFO("The device id set to " << device_id_);

	return	true;
}

bool	Endpoint::Attach(ValueID const& _device_id)
{
	device_id_ = _device_id;
	return	true;
}

bool	Endpoint::Detach(ValueID const& _device_id)
{
	if (device_id_ == _device_id)
	{
		device_id_ = ValueID("");	
	}

	return	true;
}

bool	Endpoint::Detach()
{
	device_id_ = ValueID("");	

	return	true;
}


void	Endpoint::Start()
{
	if (enable_)
	{
		if (active_)
		{
			TRACE_INFO("Endpoint starts in active mode.");
			ActiveObject::Start();
		}
		else
		{
			Device* device = manager_.GetDevice(device_id_);
			if (device != NULL)
			{
				Timer	timer;

				TRACE_INFO("Endpoint starts in passive mode.");
				stop_ = false;
				device->AddSchedule(id_, timer);
			}
			else
			{
				TRACE_ERROR("Failed to start because the endpoint is not attached to device.");
			}
		}
	}
	else
	{
		TRACE_INFO("Failed to start endpoint because endpoint is disabled!");	
	}
}

void	Endpoint::Stop()
{
	if (active_)
	{
		ActiveObject::Stop();
		TRACE_INFO("Endpoint is stopped.");
	}
	else
	{
		Device* device = manager_.GetDevice(device_id_);
		if (device != NULL)
		{
			device->RemoveSchedule(id_);
			stop_ = true;
			TRACE_INFO("Endpoint is stopped.");
		}
		else
		{
			TRACE_ERROR("Failed to stop because the endpoint is not attached to device.");
		}
	}
}

bool	Endpoint::IsRunning()
{
	if (active_)
	{
		return	ActiveObject::IsRunning();	
	}

	return	!stop_;
}

void	Endpoint::Preprocess()
{
	Date	date;

	date = Date::GetCurrentDate() + update_interval_;

	update_timer_.Set(date);
}

void	Endpoint::Process()
{
	if (update_timer_.RemainTime() == 0)
	{
		UpdateProcess();
		update_timer_ += update_interval_;	
	}
}

void	Endpoint::Postprocess()
{
}

void	Endpoint::UpdateProcess()
{
	ValueFloat	value;

	Device* device = manager_.GetDevice(device_id_);
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

bool		Endpoint::IsValid(const ValueFloat& _value)
{
	return	true;
}

uint32_t	Endpoint::GetDataCount()
{
	return	value_map_.size();
}

bool	Endpoint::GetUnreportedValueList(ValueList& value_list)
{
	Date		current;

	if (GetDataForPeriod(last_report_date_, current, value_list) == false)
	{
		TRACE_ERROR("Failed to get data!");
		return	false;
	}

	last_report_date_ = current;

	return	true;
}

Date	Endpoint::GetDateOfFirstData()
{
	ValueMap::iterator it = value_map_.begin();
	if (it == value_map_.end())
	{
		return	Date(0);	
	}
	else
	{
		return	it->first;	
	}
}

Date	Endpoint::GetDateOfLastData()
{
	ValueMap::reverse_iterator it = value_map_.rbegin();
	if (it == value_map_.rend())
	{
		return	Date(0);	
	}
	else
	{
		return	it->first;	
	}
}

bool	Endpoint::GetDataForPeriod(Date const& _begin, Date const& _end, ValueMap& _value_map)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		_value_map.Add(it->first, it->second);
	}

	return	true;
}

bool	Endpoint::GetDataForPeriod(Date const& _begin, Date const& _end, ValueList& _value_list)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		_value_list.Add(it->second);
	}

	return	true;
}

bool	Endpoint::DelDataForPeriod(Date const& _begin, Date const& _end)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		value_map_.erase(it);
	}

	return	true;
}

bool	Endpoint::Add(Value const* _value)
{
	bool	ret_value = false;

	ret_value = value_map_.Add(_value->GetDate(), _value);
	if (ret_value == true)
	{
		try
		{
			Message *message = new MessageEndpointUpdated(id_, _value);

			if (manager_.Post(message) == false)
			{
				TRACE_ERROR("Failed to post message!");	
				delete message;	
			}
		}
		catch(std::exception& e)
		{
			TRACE_ERROR("Failed to create message!");	
		}
	}
	return	ret_value;
}

bool		Endpoint::IsValidType(std::string const& _type)
{
	if ((strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_TEMPERATURE) == 0) || 
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_HUMIDITY) == 0) ||
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_VOLTAGE) == 0) ||
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_CURRENT) == 0) ||
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_DI) == 0) ||
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_PRESSURE) == 0) ||
	    (strcasecmp(_type.c_str(), ENDPOINT_TYPE_NAME_DO) == 0))
	{	
		return	true;
	}

	return	false;
}

Endpoint*	Endpoint::Create(ObjectManager& _manager, Properties const& _properties)
{

	Endpoint*	endpoint = NULL;
	const Property *type_property = _properties.Get(TITLE_NAME_TYPE);
	if (type_property != NULL)
	{
		const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
		if (type_value != NULL)
		{
			Properties	properties(_properties);

			properties.Delete(TITLE_NAME_TYPE);

			if (strcasecmp(type_value->Get().c_str(), ENDPOINT_TYPE_NAME_TEMPERATURE) == 0)
			{
				endpoint = new EndpointSensorTemperature(_manager, properties);
				TRACE_INFO2(NULL, "The temperature endpoint[" << endpoint->GetID() << "] created");
			}
			else if (strcasecmp(type_value->Get().c_str(), ENDPOINT_TYPE_NAME_HUMIDITY) == 0)
			{
				endpoint = new EndpointSensorHumidity(_manager, properties);
				TRACE_INFO2(NULL, "The humidity endpoint[" << endpoint->GetID() <<"] created");
			}
			else
			{
				TRACE_ERROR2(NULL, "Failed to create endpoint because type[" << type_value->Get().c_str() <<"] is not supported.");
			
			}
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create endpoint. Invalid arguments");
		}
	}
	else
	{
		TRACE_ERROR2(NULL, "Failed to create endpoint. Invalid arguments");
	}

	return	endpoint;
}

std::string	Endpoint::ToString(Type _type)
{
	switch(_type)
	{
	case	S_TEMPERATURE:	return	std::string(ENDPOINT_TYPE_NAME_TEMPERATURE);
	case	S_HUMIDITY:	return	std::string(ENDPOINT_TYPE_NAME_HUMIDITY);
	case	S_VOLTAGE:	return	std::string(ENDPOINT_TYPE_NAME_VOLTAGE);
	case	S_CURRENT:	return	std::string(ENDPOINT_TYPE_NAME_CURRENT);
	case	S_PRESSURE:	return	std::string(ENDPOINT_TYPE_NAME_PRESSURE);
	case	S_DI:	return	std::string(ENDPOINT_TYPE_NAME_DI);
	case	A_DO:	return	std::string(ENDPOINT_TYPE_NAME_DO);
	};

	return	std::string("unknown");
}

bool	Endpoint::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back(TITLE_NAME_ID);
	_field_list.push_back(TITLE_NAME_NAME);
	_field_list.push_back(TITLE_NAME_TYPE);
	_field_list.push_back(TITLE_NAME_DATE);
	_field_list.push_back(TITLE_NAME_ENABLE);
	_field_list.push_back(TITLE_NAME_DEVICE_ID);
	_field_list.push_back(TITLE_NAME_LOOP_INTERVAL);
	_field_list.push_back(TITLE_NAME_UNIT);
	_field_list.push_back(TITLE_NAME_SCALE);
	_field_list.push_back(TITLE_NAME_UPDATE_INTERVAL);
	_field_list.push_back(TITLE_NAME_SENSOR_ID);

	return	true;
}

const char*	ENDPOINT_TYPE_NAME_TEMPERATURE 	= "s_temperature";
const char*	ENDPOINT_TYPE_NAME_HUMIDITY 	= "s_humidity";
const char*	ENDPOINT_TYPE_NAME_VOLTAGE 		= "s_voltage";
const char*	ENDPOINT_TYPE_NAME_CURRENT 		= "s_current";
const char*	ENDPOINT_TYPE_NAME_DI 			= "s_di";
const char*	ENDPOINT_TYPE_NAME_PRESSURE 	= "s_pressure";
const char*	ENDPOINT_TYPE_NAME_DO 			= "a_do";

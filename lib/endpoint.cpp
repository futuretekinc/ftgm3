#include <list>
#include "defined.h"
#include "device.h"
#include "trace.h"
#include "value.h"
#include "endpoint.h"
#include "object_manager.h"
#include "endpoint_sensor_temperature.h"
#include "endpoint_sensor_humidity.h"

using namespace std;

static list<Endpoint*>	endpoint_list;

Endpoint::ValueMap::ValueMap(uint32_t _limit)
:	limit_(_limit)
{
	for(auto it = begin() ; it != end() ; it++)
	{
		delete it->second;	
	}
}

Endpoint::ValueMap::~ValueMap()
{
}

bool Endpoint::ValueMap::Add(Date const& _date, Value const* _value)
{
	if (_date.IsValid() && (size() < limit_))
	{
		insert(std::make_pair(_date, _value->Duplicate()));
		return	true;
	}

	return	false;
}

Endpoint::Endpoint()
:	device_(NULL), active_(false), update_interval_(ENDPOINT_UPDATE_INTERVAL), unit_(""), scale_(ENDPOINT_VALUE_SCALE), value_(0), value_map_(ENDPOINT_VALUE_COUNT_MAX)
{
	endpoint_list.push_back(this);
}

Endpoint::~Endpoint()
{
	if (device_ != NULL)
	{
		Device* device = device_;
		device_ = NULL;
		device->Detach(this);	
	}
	endpoint_list.remove(this);
}

const	ValueUnit&	Endpoint::GetUnit() const
{
	return	unit_;
}

bool	Endpoint::SetUnit(std::string const& _unit)
{
	unit_ = _unit;

	return	true;
}

float	Endpoint::GetScale() const
{
	return	scale_;
}

bool	Endpoint::SetScale(std::string const& _scale)
{
	scale_ = strtod(_scale.c_str(), NULL);

	return	true;
}

bool	Endpoint::SetScale(float _scale)
{
	scale_ = _scale;

	return	true;
}

bool	Endpoint::SetUpdateInterval(Time const& _update_interval)
{
	update_interval_ = _update_interval;

	return	true;
}

bool	Endpoint::SetUpdateInterval(uint64_t _update_interval)
{
	update_interval_ = _update_interval;

	return	true;
}

bool	Endpoint::SetUpdateInterval(std::string const& _update_interval)
{
	update_interval_ = strtoul(_update_interval.c_str(), NULL, 10);

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

bool	Endpoint::SetSensorID(std::string const& _sensor_id)
{
	sensor_id_ = _sensor_id;

	return	true;
}

bool	Endpoint::GetProperties(Properties& _properties) const
{
	if (ActiveObject::GetProperties(_properties))
	{	
		_properties.Append("type",Endpoint::ToString(GetType()));
		_properties.Append("sensor_id",sensor_id_);
		_properties.Append("device_id",((device_)?std::string(device_->GetID()):""));
		_properties.Append("unit", unit_);
		_properties.Append("scale", float(scale_));
		_properties.Append("update_interval", update_interval_);

		return	true;	
	}

	return	false;
}

bool	Endpoint::SetProperty(Property const& _property, bool create)
{
	if (_property.GetName() == "unit")
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO << "The unit of endpoint[" << id_ <<"] was set to " << string_value->Get() << Trace::End;
			return	SetUnit(string_value->Get());
		}

		TRACE_ERROR << "Property[" << id_ << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!" << Trace::End;
	}
	else if (_property.GetName() == "scale")
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO << "The scale of endpoint[" << id_ <<"] was set to " << string_value->Get() << Trace::End;
			return	SetScale(string_value->Get());
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO << "The scale of endpoint[" << id_ <<"] was set to " << float_value->Get() << Trace::End;
			return	SetScale(float_value->Get());
		}

		const ValueInt *int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO << "The scale of endpoint[" << id_ <<"] was set to " << int_value->Get() << Trace::End;
			return	SetScale(int_value->Get());
		}

		TRACE_ERROR << "Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!" << Trace::End;
	}
	else if (_property.GetName() == "update_interval")
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO << "The update interval of endpoint[" << id_ <<"] was set to " << string_value->Get() << Trace::End;
			return	SetUpdateInterval(string_value->Get());
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO << "The update interval of endpoint[" << id_ <<"] was set to " << float_value->Get() << Trace::End;
			return	SetUpdateInterval(float_value->Get());
		}

		const ValueUInt32 *int_value = dynamic_cast<const ValueUInt32*>(_property.GetValue());
		if (int_value != NULL)
		{
			TRACE_INFO << "The update interval of endpoint[" << id_ <<"] was set to " << int_value->Get() << Trace::End;
			return	SetUpdateInterval(int_value->Get());
		}

		TRACE_ERROR << "Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!" << Trace::End;
	}
	else if (_property.GetName() == "sensor_id")
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			TRACE_INFO << "The sensor id of endpoint[" << id_ <<"] was set to " << value->Get() << Trace::End;
			return	SetSensorID(value->Get());
		}

		TRACE_ERROR << "Property[" << id_ << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!" << Trace::End;
	}

	return	ActiveObject::SetProperty(_property, create);
}

Device*	Endpoint::GetDevice()
{
	return	device_;
}

bool	Endpoint::Attach(Device* _device)
{
	if (device_ != NULL)
	{
		if (device_ != _device)
		{
			device_->Detach(this);
		}
	}
	else
	{
		device_ = _device;
		device_->Attach(this);
	}

	return	true;
}

bool	Endpoint::Detach(Device* _device)
{
	if (device_ == _device)
	{
		Stop();

		Device*	device = device_;
		device_ = NULL;
		device->Detach(this);
	}

	return	true;
}


void	Endpoint::Start()
{
	if (enable_)
	{
		if (active_)
		{
			TRACE_INFO << "Endpoint[" << id_ << "] starts in active mode." << Trace::End;
			ActiveObject::Start();
		}
		else
		{
			if (device_ != NULL)
			{
				Timer	timer;

				TRACE_INFO << "Endpoint[" << id_ << "] starts in passive mode." << Trace::End;
				stop_ = false;
				device_->AddSchedule(id_, timer);
			}
			else
			{
				TRACE_ERROR << "Failed to start because the endpoint[" << id_ <<"] is not attached to device." << Trace::End;
			}
		}
	}
}

void	Endpoint::Stop()
{
	if (active_)
	{
		ActiveObject::Stop();
		TRACE_INFO << "Endpoint[" << id_ << "] is stopped." << Trace::End;
	}
	else
	{
		if (device_ != NULL)
		{
			device_->RemoveSchedule(id_);
			stop_ = true;
			TRACE_INFO << "Endpoint[" << id_ << "] is stopped." << Trace::End;
		}
		else
		{
			TRACE_ERROR << "Failed to stop because the endpoint[" << id_ <<"] is not attached to device." << Trace::End;
		}
	}
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

	if(device_ != NULL)
	{
		if (device_->ReadValue(GetID(), &value))
		{
			if (IsValid(value))
			{
				Date	date;

				if (!Add(date, &value))
				{
					TRACE_ERROR << "Failed to add data" << Trace::End;	
				}
			}
			else
			{
				TRACE_ERROR << "Data is invalid!" << Trace::End;
			}
		}
		else
		{
			TRACE_ERROR << "Failed to read data from device[" << device_->GetID() << "]" << Trace::End;	
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
}

bool	Endpoint::DelDataForPeriod(Date const& _begin, Date const& _end)
{
	ValueMap::iterator	it_low 	= value_map_.lower_bound(_begin);
	ValueMap::iterator	it_upper= value_map_.upper_bound(_end);

	for(ValueMap::iterator it = it_low ; it != it_upper ; it++)
	{
		value_map_.erase(it);
	}
}

bool	Endpoint::Add(Date const& _date, Value const* _value)
{
	if (parent_ != NULL)
	{
		parent_->AddData(id_, _date, _value);
	}
	return	value_map_.Add(_date, _value);
}

bool		Endpoint::IsValidType(std::string const& _type)
{
	if ((strcasecmp(_type.c_str(), "s_temperature") == 0) || 
	    (strcasecmp(_type.c_str(), "s_humidity") == 0) ||
	    (strcasecmp(_type.c_str(), "s_voltage") == 0) ||
	    (strcasecmp(_type.c_str(), "s_current") == 0) ||
	    (strcasecmp(_type.c_str(), "s_di") == 0) ||
	    (strcasecmp(_type.c_str(), "s_pressure") == 0) ||
	    (strcasecmp(_type.c_str(), "a_do") == 0))
	{	
		return	true;
	}

	return	false;
}

Endpoint*	Endpoint::Create(Properties const& _properties)
{

	Endpoint*	endpoint = NULL;
	const Property *type_property = _properties.Get("type");
	if (type_property != NULL)
	{
		const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
		if (type_value != NULL)
		{
			Properties	properties(_properties);

			properties.Delete("type");

			if (strcasecmp(type_value->Get().c_str(), "s_temperature") == 0)
			{
				endpoint = new EndpointSensorTemperature(properties);
				TRACE_INFO2 << "The temperature endpoint[" << endpoint->GetID() << "] created" << Trace::End;
			}
			else if (strcasecmp(type_value->Get().c_str(), "s_humidity") == 0)
			{
				endpoint = new EndpointSensorHumidity(properties);
				TRACE_INFO2 << "The humidity endpoint[" << endpoint->GetID() <<"] created" << Trace::End;
			}
			else
			{
				TRACE_ERROR2 << "Failed to create endpoint because type[" << type_value->Get().c_str() <<"] is not supported." << Trace::End;
			
			}
		}
		else
		{
			TRACE_ERROR2 << "Failed to create endpoint. Invalid arguments" << Trace::End;
		}
	}
	else
	{
		TRACE_ERROR2 << "Failed to create endpoint. Invalid arguments" << Trace::End;
	}

	return	endpoint;
}

Endpoint*	Endpoint::Get(std::string const& _id)
{
	for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
	{
		if ((*it)->id_ == _id)
		{
			return	(*it);	
		}
	}

	return	NULL;
}

uint32_t	Endpoint::Count()
{
	return	endpoint_list.size();
}

uint32_t	Endpoint::GetList(std::list<Endpoint*>& _endpoint_list)
{
	for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
	{
		_endpoint_list.push_back(*it);
	}

	return	_endpoint_list.size();
}

uint32_t	Endpoint::UnattachedCount()
{
	uint32_t	count = 0;

	for(auto it = endpoint_list.begin(); it != endpoint_list.end() ; it++)
	{
		if ((*it)->device_ == 0)
		{
			count++;
		}
	}

	return	count;
}

std::string	Endpoint::ToString(Type _type)
{
	switch(_type)
	{
	case	S_TEMPERATURE:	return	std::string("s_temperature");
	case	S_HUMIDITY:	return	std::string("s_humidity");
	case	S_VOLTAGE:	return	std::string("s_voltage");
	case	S_CURRENT:	return	std::string("s_current");
	case	S_PRESSURE:	return	std::string("s_pressure");
	case	S_DI:	return	std::string("s_di");
	case	A_DO:	return	std::string("a_do");
	};

	return	std::string("unknown");
}

bool	Endpoint::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	_field_list.push_back("id");
	_field_list.push_back("name");
	_field_list.push_back("type");
	_field_list.push_back("date");
	_field_list.push_back("enable");
	_field_list.push_back("device_id");
	_field_list.push_back("loop_interval");
	_field_list.push_back("unit");
	_field_list.push_back("scale");
	_field_list.push_back("update_interval");
	_field_list.push_back("sensor_id");

	return	true;
}

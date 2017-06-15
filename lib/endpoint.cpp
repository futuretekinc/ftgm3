#include <list>
#include <libjson/libjson.h>
#include "defined.h"
#include "device.h"
#include "trace.h"
#include "value.h"
#include "endpoint.h"
#include "object_manager.h"
#include "endpoint_sensor_temperature.h"
#include "endpoint_sensor_humidity.h"
#include "utils.h"

EndpointInfo::EndpointInfo()
: NodeInfo(), sensor_id(""), unit(""), scale(ENDPOINT_VALUE_SCALE)
{
}

EndpointInfo::EndpointInfo(JSONNode const& _json)
: NodeInfo(_json), sensor_id(""), unit(""), scale(ENDPOINT_VALUE_SCALE)
{
	std::string	value;

	if (GetValue(_json, TITLE_NAME_SENSOR_ID, value, false))
	{
		sensor_id = value;
		fields.sensor_id = true;
	}

	if (GetValue(_json, TITLE_NAME_UNIT, value, false))
	{
		id = value;
		fields.unit = true;
	}

	if (GetValue(_json, TITLE_NAME_SCALE, scale, false))
	{
		fields.scale= true;
	}
}

bool	EndpointInfo::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	if (!NodeInfo::GetProperties(_properties, _fields))
	{
		return	false;
	}

	if (_fields.sensor_id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_SENSOR_ID, sensor_id));
	}

	if (_fields.unit)
	{
		_properties.push_back(JSONNode(TITLE_NAME_UNIT, unit));
	}

	if (_fields.scale)
	{
		_properties.push_back(JSONNode(TITLE_NAME_SCALE, float(scale)));
	}

	return	true;
}

Endpoint::Endpoint(ObjectManager& _manager, ValueType const& _type)
:	Node(_manager, _type),
	active_(false), 
	unit_(""), 
	scale_(ENDPOINT_VALUE_SCALE), 
	value_(0), 
	value_map_(ENDPOINT_VALUE_COUNT_MAX),
	last_report_date_()
{
	keep_alive_interval_ = 0;
}

Endpoint::~Endpoint()
{
}

const	ValueUnit&	Endpoint::GetUnit() const
{
	return	unit_;
}

bool	Endpoint::SetUnit(std::string const& _unit)
{
	unit_ = _unit;
	
	updated_properties_.AppendUnit(unit_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

float	Endpoint::GetScale() const
{
	return	scale_;
}

bool	Endpoint::SetScale(std::string const& _scale)
{
	scale_ = strtod(_scale.c_str(), NULL);

	updated_properties_.AppendScale(scale_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::SetScale(float _scale)
{
	scale_ = _scale;

	updated_properties_.AppendScale(scale_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

std::string	Endpoint::GetSensorID() const
{
	return	sensor_id_;
}

bool	Endpoint::SetSensorID(std::string const& _sensor_id)
{
	sensor_id_ = _sensor_id;

	updated_properties_.AppendSensorID(sensor_id_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::GetProperties(Properties& _properties, Properties::Fields const& _fields)
{
	if (Node::GetProperties(_properties, _fields))
	{	
		if (_fields.sensor_id)
		{
			_properties.AppendSensorID(sensor_id_);
		}

		if (_fields.unit)
		{
			_properties.AppendUnit(unit_);
		}

		if (_fields.scale)
		{
			_properties.AppendScale(float(scale_));
		}

		return	true;	
	}

	return	false;
}

bool	Endpoint::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	return	Node::GetProperties(_properties, _fields);
}

bool	Endpoint::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	if (_property.GetName() == TITLE_NAME_UNIT)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The unit set to " << string_value->Get());
			return	SetUnit(string_value->Get());
		}

		TRACE_ERROR("Property[" << id_ << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_SCALE)
	{
		const ValueString*	string_value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (string_value != NULL)
		{
			TRACE_INFO("The scale set to " << string_value->Get());
			return	SetScale(string_value->Get());
		}

		const ValueFloat *float_value = dynamic_cast<const ValueFloat*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The scale set to " << float_value->Get());
			return	SetScale(float_value->Get());
		}

		const ValueInt *int_value = dynamic_cast<const ValueInt*>(_property.GetValue());
		if (float_value != NULL)
		{
			TRACE_INFO("The scale set to " << int_value->Get());
			return	SetScale(int_value->Get());
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}
	else if (_property.GetName() == TITLE_NAME_SENSOR_ID)
	{
		const ValueString*	value = dynamic_cast<const ValueString*>(_property.GetValue());
		if (value != NULL)
		{
			TRACE_INFO("The sensor id set to " << value->Get());
			return	SetSensorID(value->Get());
		}

		TRACE_ERROR("Property[" << _property.GetName() << "] value type[" << _property.GetValue()->GetTypeString() << "] invalid!");
	}

	return	Node::SetProperty(_property, _fields);
}

Endpoint::operator JSONNode()
{
	JSONNode	properties;

	Object::GetProperties(properties);

	return	properties;
}

bool	Endpoint::Attach(ValueID const& _parent_id)
{
	parent_id_ = _parent_id;
	return	true;
}

bool	Endpoint::Detach(ValueID const& _parent_id)
{
	if (parent_id_ == _parent_id)
	{
		parent_id_ = ValueID("");	
	}

	return	true;
}

bool	Endpoint::Detach()
{
	parent_id_ = ValueID("");	

	return	true;
}


bool	Endpoint::Start()
{
	if (enable_)
	{
		if (active_)
		{
			TRACE_INFO("Endpoint starts in active mode.");
			Node::Start();
		}
		else
		{
			if (stop_)
			{
				Device* device = manager_.GetDevice(parent_id_ );
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
					return	false;
				}
			}
			else
			{
				TRACE_INFO("Endpoint[" << id_ << "] already started!");
			}
		}
	}
	else
	{
		TRACE_INFO("Failed to start endpoint because endpoint is disabled!");	
		return	false;
	}

	return	true;
}

bool	Endpoint::Stop(bool _wait)
{
	bool	ret_value = true;

	if (active_)
	{
		ret_value = Node::Stop(_wait);
		TRACE_INFO("Endpoint is stopped.");
	}
	else
	{
		Device* device = manager_.GetDevice(parent_id_);
		if (device != NULL)
		{
			device->RemoveSchedule(id_);
			stop_ = true;
			TRACE_INFO("Endpoint is stopped.");
		}
		else
		{
			TRACE_ERROR("Failed to stop because the endpoint is not attached to device.");
			ret_value = false;
		}
	}

	return	ret_value;
}

bool	Endpoint::IsRunning()
{
	if (active_)
	{
		return	Node::IsRunning();	
	}

	return	!stop_;
}

void	Endpoint::Preprocess()
{
	Node::Preprocess();
}

void	Endpoint::Process()
{
	Node::Process();
}

void	Endpoint::Postprocess()
{
	Node::Postprocess();
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

bool	Endpoint::SetLastReportTime(Date const& _time)
{
	last_report_date_ = _time;

	return	0;
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
			Message *message = new MessageEndpointUpdated(id_, id_, _value);

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
	if ((strcasecmp(_type.c_str(), NODE_TYPE_EP_S_TEMPERATURE) == 0) || 
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_HUMIDITY) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_VOLTAGE) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_CURRENT) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_DI) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_PRESSURE) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_A_DO) == 0))
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

			if (strcasecmp(type_value->Get().c_str(), NODE_TYPE_EP_S_TEMPERATURE) == 0)
			{
				endpoint = new EndpointSensorTemperature(_manager, properties);
				TRACE_INFO2(NULL, "The temperature endpoint[" << endpoint->GetID() << "] created");
			}
			else if (strcasecmp(type_value->Get().c_str(), NODE_TYPE_EP_S_HUMIDITY) == 0)
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
		TRACE_ERROR2(NULL, "Failed to create endpoint. Invalid arguments[" << type_property->GetName() << "]");
	}

	return	endpoint;
}

bool	Endpoint::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Node::GetPropertyFieldList(_field_list))
	{
		_field_list.push_back(TITLE_NAME_UNIT);
		_field_list.push_back(TITLE_NAME_SCALE);
		_field_list.push_back(TITLE_NAME_CORRECTION_INTERVAL);
		_field_list.push_back(TITLE_NAME_SENSOR_ID);
	}

	return	true;
}


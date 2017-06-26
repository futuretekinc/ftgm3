#include <list>
#include <libjson/libjson.h>
#include "defined.h"
#include "exception.h"
#include "json.h"
#include "device.h"
#include "trace.h"
#include "value.h"
#include "endpoint.h"
#include "object_manager.h"
#include "endpoint_sensor_temperature.h"
#include "endpoint_sensor_humidity.h"
#include "endpoint_sensor_voltage.h"
#include "endpoint_sensor_current.h"
#include "endpoint_sensor_di.h"
#include "endpoint_sensor_windspeed.h"
#include "endpoint_sensor_soilmoisture.h"
#include "endpoint_sensor_soil_acidity.h"
#include "endpoint_sensor_rainfall.h"
#include "endpoint_sensor_gas.h"
#include "endpoint_actuator_do.h"
#include "utils.h"

Endpoint::Endpoint(ObjectManager& _manager, std::string const& _type)
:	Node(_manager, _type),
	active_(false), 
	unit_(""), 
	scale_(ENDPOINT_VALUE_SCALE), 
	correction_interval_(ENDPOINT_UPDATE_INTERVAL),
	value_map_(ENDPOINT_VALUE_COUNT_MAX),
	last_report_date_(),
	last_confirm_date_()
{
}

Endpoint::Endpoint(ObjectManager& _manager, std::string const& _type, std::string const& _unit)
: Node(_manager, _type),
	active_(false), 
	scale_(ENDPOINT_VALUE_SCALE), 
	correction_interval_(ENDPOINT_UPDATE_INTERVAL),
	value_map_(ENDPOINT_VALUE_COUNT_MAX),
	last_report_date_(),
	last_confirm_date_(),
	unit_(_unit)
{
}

Endpoint::~Endpoint()
{
}

const std::string&	Endpoint::GetUnit() const
{
	return	unit_;
}

bool	Endpoint::SetUnit(std::string const& _unit, bool _check)
{
	if (!_check)
	{
		unit_ = _unit;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_UNIT, unit_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

float	Endpoint::GetScale() const
{
	return	scale_;
}

bool	Endpoint::SetScale(std::string const& _scale, bool _check)
{
	if (!_check)
	{
		scale_ = strtod(_scale.c_str(), NULL);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_SCALE, scale_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}
	return	true;
}

bool	Endpoint::SetScale(float _scale)
{
	scale_ = _scale;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_SCALE, scale_);

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

bool	Endpoint::SetSensorID(std::string const& _sensor_id, bool _check)
{
	if (!_check)
	{
		sensor_id_ = _sensor_id;

		JSONNodeUpdate(updated_properties_, TITLE_NAME_SENSOR_ID, sensor_id_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}
	return	true;
}

bool	Endpoint::SetCorrectionInterval(Time const& _interval)
{
	correction_interval_ = _interval;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::SetCorrectionInterval(uint32_t _interval)
{
	correction_interval_ = _interval;

	JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);

	if (!lazy_store_)
	{
		ApplyChanges();	
	}

	return	true;
}

bool	Endpoint::SetCorrectionInterval(std::string const& _interval, bool _check)
{
	if (!_check)
	{
		correction_interval_ = strtoul(_interval.c_str(), NULL, 10);

		JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);

		if (!lazy_store_)
		{
			ApplyChanges();	
		}
	}

	return	true;
}

uint32_t	Endpoint::GetCorrectionInterval()
{
	return	correction_interval_;
}

bool	Endpoint::GetProperties(JSONNode& _properties, Fields const& _fields)
{
	if (!Node::GetProperties(_properties, _fields))
	{	
		return	false;
	}

	if (_fields.sensor_id)
	{
		_properties.push_back(JSONNode(TITLE_NAME_SENSOR_ID, sensor_id_));
	}

	if (_fields.unit)
	{
		_properties.push_back(JSONNode(TITLE_NAME_UNIT, unit_));
	}

	if (_fields.scale)
	{
		_properties.push_back(JSONNode(TITLE_NAME_SCALE, scale_));
	}

	if (_fields.correction_interval)
	{
		_properties.push_back(JSONNode(TITLE_NAME_CORRECTION_INTERVAL, correction_interval_));
	}

	if (_fields.value)
	{
		_properties.push_back(JSONNode(TITLE_NAME_VALUE, GetValue()));
	}

	return	true;	
}

bool	Endpoint::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_UNIT)
	{
		ret_value = SetUnit(_property.as_string(), _check);
	}
	else if (_property.name() == TITLE_NAME_SCALE)
	{
		ret_value = SetScale(_property.as_string(), _check);
	}
	else if (_property.name() == TITLE_NAME_SENSOR_ID)
	{
		ret_value = SetSensorID(_property.as_string(), _check);
	}
	else if ((_property.name() == TITLE_NAME_CORRECTION_INTERVAL) || (_property.name() == TITLE_NAME_UPDATE_INTERVAL))
	{
		ret_value = SetCorrectionInterval(_property.as_string(), _check);
	}
	else
	{
		ret_value = Node::SetProperty(_property, _check);
	}

	return	ret_value;
}

Endpoint::operator JSONNode()
{
	JSONNode	properties;

	Object::GetProperties(properties);

	return	properties;
}

bool	Endpoint::Attach(std::string const& _parent_id)
{
	parent_id_ = _parent_id;
	return	true;
}

bool	Endpoint::Detach(std::string const& _parent_id)
{
	if (parent_id_ == _parent_id)
	{
		parent_id_ = "";	
	}

	return	true;
}

bool	Endpoint::Detach()
{
	parent_id_ = "";	

	return	true;
}


bool	Endpoint::Start(uint32_t _wait_for_init_time)
{
	if (enable_)
	{
		if (active_)
		{
			TRACE_INFO("Endpoint starts in active mode.");
			Node::Start(_wait_for_init_time);
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

bool		Endpoint::IsValid(std::string const& _value)
{
	return	true;
}

std::string	Endpoint::GetValue()
{
	return	std::string("");
}

void	Endpoint::CorrectionProcess()
{
	std::string	value;
	time_t		time;

	Device* device = manager_.GetDevice(parent_id_);
	if(device != NULL)
	{
		if (device->ReadValue(GetID(), time, value))
		{
			if (IsValid(value))
			{
				if (!Add(time, value))
				{
					TRACE_ERROR("Failed to add data");	
				}
			}
			else
			{
				TRACE_ERROR("Data[" << value << "] is invalid!");
			}
		}
		else
		{
			TRACE_ERROR("Failed to read data from device[" << device->GetTraceName() << "]");	
		}
	}
}


uint32_t	Endpoint::GetDataCount()
{
	return	value_map_.size();
}

bool	Endpoint::GetUnreportedValueMap(ValueMap& value_map)
{
	Date		current;

	if (GetDataForPeriod(last_report_date_, current, value_map) == false)
	{
		TRACE_ERROR("Failed to get data!");
		return	false;
	}

	last_report_date_ = current;

	return	true;
}

bool	Endpoint::SetLastReportTime(Date const& _time)
{
	if (time_t(last_report_date_) < time_t(_time))
	{
		last_report_date_ = _time;
	}

	return	0;
}

bool	Endpoint::SetLastConfirmTime(Date const& _time)
{
	if (time_t(last_confirm_date_) < time_t(_time))
	{
		last_confirm_date_ = _time;
	}

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

uint32_t	Endpoint::GetData(uint32_t _count, ValueMap& _value_map)
{
	for(ValueMap::reverse_iterator it = value_map_.rbegin() ; _count > 0 && it != value_map_.rend() ; _count--, it++)
	{
		_value_map.Add(it->first, it->second);	
	}

	return	_value_map.size();
}

bool	Endpoint::Add(time_t time, std::string const& _value)
{
	bool	ret_value = false;

	ret_value = value_map_.Add(time, _value);
	if (ret_value == true)
	{
#if 0
		try
		{
			Message *message = new MessageEndpointUpdated(id_, id_, time, _value);

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
#endif
	}
	return	ret_value;
}

bool	Endpoint::Add(time_t time, bool _value)
{
	std::string	value = ::ToString(_value);

	return	Add(time, value);
}

bool		Endpoint::IsValidType(std::string const& _type)
{
	if ((strcasecmp(_type.c_str(), NODE_TYPE_EP_S_TEMPERATURE) == 0) || 
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_HUMIDITY) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_VOLTAGE) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_CURRENT) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_DI) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_PRESSURE) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_WIND_SPEED) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_SOIL_MOISTURE) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_RAINFALL) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_S_GAS) == 0) ||
	    (strcasecmp(_type.c_str(), NODE_TYPE_EP_A_DO) == 0))
	{	
		return	true;
	}

	return	false;
}

Endpoint*	Endpoint::Create(ObjectManager& _manager, JSONNode const& _properties)
{

	Endpoint*	endpoint = NULL;
	try
	{
		TRACE_INFO2(&_manager, "Endpoint Properties : " << _properties.write_formatted());

		std::string type = JSONNodeGetType(_properties);

		if (type == NODE_TYPE_EP_S_TEMPERATURE)
		{
			endpoint = new EndpointSensorTemperature(_manager, _properties);
			TRACE_INFO2(NULL, "The temperature endpoint[" << endpoint->GetID() << "] created");
		}
		else if (type == NODE_TYPE_EP_S_HUMIDITY) 
		{
			endpoint = new EndpointSensorHumidity(_manager, _properties);
			TRACE_INFO2(NULL, "The humidity endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_VOLTAGE)
		{
			endpoint = new EndpointSensorVoltage(_manager, _properties);
			TRACE_INFO2(NULL, "The voltage endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_CURRENT)
		{
			endpoint = new EndpointSensorCurrent(_manager, _properties);
			TRACE_INFO2(NULL, "The current endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_DI) 
		{
			endpoint = new EndpointSensorDI(_manager, _properties);
			TRACE_INFO2(NULL, "The DI endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_WIND_SPEED) 
		{
			endpoint = new EndpointSensorWindSpeed(_manager, _properties);
			TRACE_INFO2(NULL, "The Wind Spped endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_SOIL_MOISTURE) 
		{
			endpoint = new EndpointSensorSoilMoisture(_manager, _properties);
			TRACE_INFO2(NULL, "The soil moisture endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_SOIL_ACIDITY) 
		{
			endpoint = new EndpointSensorSoilAcidity(_manager, _properties);
			TRACE_INFO2(NULL, "The soil acidity endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_RAINFALL) 
		{
			endpoint = new EndpointSensorRainfall(_manager, _properties);
			TRACE_INFO2(NULL, "The railfall endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_S_GAS) 
		{
			endpoint = new EndpointSensorGAS(_manager, _properties);
			TRACE_INFO2(NULL, "The gas endpoint[" << endpoint->GetID() <<"] created");
		}
		else if (type == NODE_TYPE_EP_A_DO)
		{
			endpoint = new EndpointActuatorDO(_manager, _properties);
			TRACE_INFO2(NULL, "The DO endpoint[" << endpoint->GetID() <<"] created");
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create endpoint because type[" << type <<"] is not supported.");

		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR2(NULL, "Failed to create endpoint. Invalid arguments");
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


bool	Endpoint::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<Endpoint *>(_object) != NULL;
}

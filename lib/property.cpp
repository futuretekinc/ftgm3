#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "defined.h"
#include "property.h"
#include "gateway.h"
#include "device.h"
#include "endpoint.h"
 
Properties::Fields	PROPERTY_CORRECTION_INTERVAL( true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_DATE				(false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_DEV_NAME			(false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_ENABLE				(false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_ID					(false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_IP					(false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_KEEP_ALIVE_INTERVAL(false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_LOCATION			(false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_LOOP_INTERVAL		(false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_NAME				(false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_PARENT_ID			(false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_REGISTERED			(false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_SCALE				(false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_SENSOR_ID			(false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_SNMP_MODULE		(false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_SNMP_COMMUNITY		(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_TIME				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false);
Properties::Fields	PROPERTY_TIMEOUT			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false);
Properties::Fields	PROPERTY_TYPE				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false);
Properties::Fields	PROPERTY_VALUE_MIN			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false);
Properties::Fields	PROPERTY_VALUE_MAX			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false);
Properties::Fields	PROPERTY_UNIT				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false);
Properties::Fields	PROPERTY_UPDATE_INTERVAL	(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false);
Properties::Fields	PROPERTY_TIME_OF_EXPIRE		(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true);
Properties::Fields	PROPERTY_ALL				( true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Properties::Fields	PROPERTY_CREATE				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Properties::Fields	PROPERTY_ADD_CONFIRM		( true, false,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Properties::Fields	PROPERTY_GET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Properties::Fields	PROPERTY_SET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);

Properties::Fields::Fields()
{
	correction_interval = false;
	date = false;
	dev_name = false;
	enable = false;
	id = false;
	ip = false;
	keep_alive_interval = false;
	location = false;
	loop_interval = false;
	name = false;
	parent_id = false;
	registered = false;
	scale = false;
	sensor_id = false;
	snmp_module = false;
	snmp_community = false;
	time = false;
	timeout = false;
	type = false;
	value_min = false;
	value_max = false;
	unit = false;
	update_interval = false;
	time_of_expire = false;
}

Properties::Fields::Fields( bool	_correction_interval, bool	_date, bool	_dev_name, bool	_enable, bool	_id, bool	_ip, bool	_keep_alive_interval, bool	_location, bool	_loop_interval, bool	_name,
				bool	_parent_id, bool	_registered, bool	_scale, bool	_sensor_id, bool	_snmp_module, bool	_snmp_community, bool	_time, bool	_timeout, bool	_type, bool	_value_min,
				bool	_value_max, bool	_unit, bool	_update_interval, bool _time_of_expire)
:correction_interval(_correction_interval), date(date), dev_name(_dev_name), enable(_enable), id(_id), ip(_ip), keep_alive_interval(_keep_alive_interval), location(_location), loop_interval(_loop_interval), name(_name),
				parent_id(_parent_id), registered(_registered), scale(_scale), sensor_id(_sensor_id), snmp_module(_snmp_module), snmp_community(_snmp_community), time(_time), timeout(_timeout), type(_type),
				value_min(_value_min), value_max(_value_max), unit(_unit), update_interval(_update_interval), time_of_expire(_time_of_expire)
{
}

Properties::Fields	operator+(Properties::Fields& _field1, Properties::Fields& _fields2)
{
	Properties::Fields	fields;

	fields.correction_interval = _field1.correction_interval || _fields2.correction_interval;
	fields.date = _field1.date || _fields2.date;
	fields.dev_name = _field1.dev_name || _fields2.dev_name;
	fields.enable = _field1.enable || _fields2.enable;
	fields.id = _field1.id || _fields2.id;
	fields.ip = _field1.ip || _fields2.ip;
	fields.keep_alive_interval = _field1.keep_alive_interval || _fields2.keep_alive_interval;
	fields.location = _field1.location || _fields2.location;
	fields.loop_interval = _field1.loop_interval || _fields2.loop_interval;
	fields.name = _field1.name || _fields2.name;
	fields.parent_id = _field1.parent_id || _fields2.parent_id;
	fields.registered = _field1.registered || _fields2.registered;
	fields.scale = _field1.scale || _fields2.scale;
	fields.sensor_id = _field1.sensor_id || _fields2.sensor_id;
	fields.snmp_module = _field1.snmp_module || _fields2.snmp_module;
	fields.snmp_community = _field1.snmp_community || _fields2.snmp_community;
	fields.time = _field1.time || _fields2.time;
	fields.timeout = _field1.timeout || _fields2.timeout;
	fields.type = _field1.type || _fields2.type;
	fields.value_min = _field1.value_min || _fields2.value_min;
	fields.value_max = _field1.value_max || _fields2.value_max;
	fields.unit = _field1.unit ||  _fields2.unit;
	fields.update_interval = _field1.update_interval || _fields2.update_interval;
	fields.time_of_expire = _field1.time_of_expire || _fields2.time_of_expire;

	return	fields;
}

Properties::Fields&	Properties::Fields::operator+=(Properties::Fields& _fields)
{
	correction_interval = correction_interval || _fields.correction_interval;
	date = date || _fields.date;
	dev_name = dev_name || _fields.dev_name;
	enable = enable || _fields.enable;
	id = id || _fields.id;
	ip = ip || _fields.ip;
	keep_alive_interval = keep_alive_interval || _fields.keep_alive_interval;
	location = location || _fields.location;
	loop_interval = loop_interval || _fields.loop_interval;
	name = name || _fields.name;
	parent_id = parent_id || _fields.parent_id;
	registered = registered || _fields.registered;
	scale = scale || _fields.scale;
	sensor_id = sensor_id || _fields.sensor_id;
	snmp_module = snmp_module || _fields.snmp_module;
	snmp_community = snmp_community || _fields.snmp_community;
	time = time || _fields.time;
	timeout = timeout || _fields.timeout;
	type = type || _fields.type;
	value_min = value_min || _fields.value_min;
	value_max = value_max || _fields.value_max;
	unit = unit ||  _fields.unit;
	update_interval = update_interval || _fields.update_interval;
	time_of_expire = time_of_expire || _fields.time_of_expire;

	return	*this;
}

bool	Properties::Fields::Set(std::string const& _field)
{
	if (_field == TITLE_NAME_CORRECTION_INTERVAL)
	{
		correction_interval = true;
	}
	else if (_field == TITLE_NAME_DATE)
	{
		date = true;
	}
	else if (_field == TITLE_NAME_DEV_NAME)
	{
		dev_name = true;
	}
	else if (_field == TITLE_NAME_ENABLE)
	{
		enable = true;
	}
	else if (_field == TITLE_NAME_ID)
	{
		id = true;
	}
	else if (_field == TITLE_NAME_IP)
	{
		ip = true;
	}
	else if (_field == TITLE_NAME_KEEP_ALIVE_INTERVAL)
	{
		keep_alive_interval = true;
	}
	else if (_field == TITLE_NAME_LOCATION)
	{
		location = true;
	}
	else if (_field == TITLE_NAME_LOOP_INTERVAL)
	{
		loop_interval = true;
	}
	else if (_field == TITLE_NAME_NAME)
	{
		name = true;
	}
	else if (_field == TITLE_NAME_PARENT_ID)
	{
		parent_id = true;
	}
	else if (_field == TITLE_NAME_REGISTERED)
	{
		registered = true;
	}
	else if (_field == TITLE_NAME_SCALE)
	{
		scale = true;
	}
	else if (_field == TITLE_NAME_SENSOR_ID)
	{
		sensor_id = true;
	}
	else if (_field == TITLE_NAME_MODULE)
	{
		snmp_module = true;
	}
	else if (_field == TITLE_NAME_COMMUNITY)
	{
		snmp_community = true;
	}
	else if (_field == TITLE_NAME_TIME)
	{
		time = true;
	}
	else if (_field == TITLE_NAME_TIMEOUT)
	{
		timeout = true;
	}
	else if (_field == TITLE_NAME_TYPE)
	{
		type = true;
	}
	else if (_field == TITLE_NAME_VALUE_MIN)
	{
		value_min = true;
	}
	else if (_field == TITLE_NAME_VALUE_MAX)
	{
		value_max = true;
	}
	else if (_field == TITLE_NAME_UNIT)
	{
		unit = true;
	}
	else if (_field == TITLE_NAME_UPDATE_INTERVAL)
	{
		update_interval = true;
	}
	else if (_field == TITLE_NAME_TIME_OF_EXPIRE)
	{
		time_of_expire = true;
	}
	else
	{
		return	false;	
	}

	return	true;
}


Property::Property(Property const& _property)
{
	first = _property.first;
	second = _property.second->Duplicate();
}

Property::Property(std::string const& _name, int _value)
{
	first = _name;
	second = new ValueInt(_value);
}

Property::Property(std::string const& _name, uint32_t _value)
{
	first = _name;
	second = new ValueUInt32(_value);
}

Property::Property(std::string const& _name, uint64_t _value)
{
	first = _name;
	second = new ValueUInt64(_value);
}

Property::Property(std::string const& _name, bool _value)
{
	first = _name;
	second = new ValueBool(_value);
}

Property::Property(std::string const& _name, float _value)
{
	first = _name;
	second = new ValueFloat(_value);
}

Property::Property(std::string const& _name, double _value)
{
	first = _name;
	second = new ValueFloat(_value);
}

Property::Property(std::string const& _name, char const* _value)
{
	first = _name;
	second = new ValueString(_value);
}

Property::Property(std::string const& _name, std::string const& _value)
{
	first = _name;
	second = new ValueString(_value);
}

Property::Property(std::string const& _name, Date const& _value)
{
	first = _name;
	second = new ValueDate(_value);
}

Property::Property(std::string const& _name, Time const& _value)
{
	first = _name;
	second = new ValueTime(_value);
}

Property::Property(std::string const& _name, time_t _value)
{
	first = _name;
	second = new ValueTime(_value);
}

Property::Property(std::string const& _name, Properties const& _value)
{
	first = _name;
	second = new ValueProperties(_value);
}

Property::Property(std::string const& _name, PropertiesList const& _value)
{
	first = _name;
	second = new ValuePropertiesList(_value);
}

Property::Property(std::string const& _name, ValueFloat const& _value)
{
	first = _name;
	second = new ValueFloat(_value);
}

Property::~Property()
{
	delete second;
}

const Property& Property::operator=(Property const& _property)
{
	if (second != NULL)
	{
		delete second;	
	}

	first = _property.first;
	second = _property.second->Duplicate();
}

bool	Property::operator=(std::string const& _value)
{
	return	second->Set(_value);
}

bool	Property::Set(std::string const& _value)
{
	return	second->Set(_value);
}

const std::string&	Property::GetName() const
{
	return	first;
}

const Value*		Property::GetValue() const
{
	return	second;
}

std::ostream&	operator<<(std::ostream& os, Property const& _property)
{

	return	os << JSONNode(_property).write_formatted();
}

Property::operator JSONNode() const
{
	JSONNode	json;

	if (dynamic_cast<ValueInt*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueInt*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt32*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueUInt32*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueUInt64*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueUInt64*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueFloat*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueFloat*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueBool*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueBool*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueString*>(second))
	{
		json = JSONNode(first,dynamic_cast<ValueString*>(second)->Get()); 
	}
	else if (dynamic_cast<ValueDate*>(second))
	{
		ValueDate *value = dynamic_cast<ValueDate*>(second);
		Date	date = value->Get();
		time_t	time = time_t(date);
	
		json = JSONNode(first,time);
	}
	else if (dynamic_cast<ValueTime*>(second))
	{
		ValueTime *time = dynamic_cast<ValueTime*>(second);
		json = JSONNode(first,time_t(time->Get()));
	}
	else if (dynamic_cast<ValueProperties*>(second))
	{	
		ValueProperties *value = dynamic_cast<ValueProperties*>(second);
		
		value->Get().ToJSON(json);

		json.set_name(first);
	}
	else if (dynamic_cast<ValuePropertiesList*>(second))
	{	
		ValuePropertiesList *value = dynamic_cast<ValuePropertiesList*>(second);

		json = JSONNode(value->Get());

		json.set_name(first);
	}


	return	json;
}


///////////////////////////////////////////////////////////////////////////
// Class	Properties
///////////////////////////////////////////////////////////////////////////

Properties::Properties()
{
}

Properties::Properties(Properties const& _properties)
{
	for(auto it = _properties.begin() ; it != _properties.end() ; it++)
	{
		Append(*it);			
	}
}

Properties::~Properties()
{
}

bool	Properties::Append(Property const& _property)
{
	if (Get(_property.GetName()) != NULL)
	{
		Delete(_property.GetName());
	//	return	false;
	}

	push_back(_property);
	return	true;
}

bool Properties::Append(JSONNode const& _json)
{
	switch(_json.type())
	{
	case	JSON_BOOL: 	
		{
			Append(Property(_json.name(), 	_json.as_bool()));	
		}
		break;
	case	JSON_NUMBER: 	
		{
			Append(Property(_json.name(),	(uint32_t)_json.as_int()));	
		}
		break;

	case	JSON_STRING:
		{
			Append(Property(_json.name(), 	_json.as_string()));	
		}
		break;

	case	JSON_NODE: 	
		{	
			for(auto it = _json.begin(); it != _json.end() ; it++)
			{
				Append(*it);	
			}
		}
		break;

	case	JSON_ARRAY:
		{
			Append(Property(_json.name(), PropertiesList(_json.as_array())));
		}	
	}

	return	true;
}

bool	Properties::AppendCorrectionInterval(uint32_t _interval)
{
	return	Append(Property(TITLE_NAME_CORRECTION_INTERVAL, _interval));
}

bool	Properties::AppendDate(Date const& _date)
{
	Delete(TITLE_NAME_TIME);

	return	Append(Property(TITLE_NAME_TIME, _date));
}

bool	Properties::AppendDevName(std::string const& _dev_name)
{
	Delete(TITLE_NAME_DEV_NAME);

	return	Append(Property(TITLE_NAME_DEV_NAME, _dev_name));
}

bool	Properties::AppendEnable(ValueBool const& _enable)
{
	Delete(TITLE_NAME_ENABLE);

	return	Append(Property(TITLE_NAME_ENABLE, bool(_enable)));
}

bool	Properties::AppendEnable(bool _enable)
{
	Delete(TITLE_NAME_ENABLE);

	return	Append(Property(TITLE_NAME_ENABLE, _enable));
}

bool	Properties::AppendID(ValueID const& _id)
{
	Delete(TITLE_NAME_ID);

	return	Append(Property(TITLE_NAME_ID, _id));
}

bool	Properties::AppendIP(ValueIP const& _ip)
{
	return	Append(Property(TITLE_NAME_IP, _ip));
}

bool	Properties::AppendKeepAliveInterval(uint32_t _interval)
{
	return	Append(Property(TITLE_NAME_KEEP_ALIVE_INTERVAL, _interval));
}

bool	Properties::AppendLocation(std::string const& _location)
{
	return	Append(Property(TITLE_NAME_LOCATION, _location));
}

bool	Properties::AppendLoopInterval(uint32_t _interval)
{
	return	Append(Property(TITLE_NAME_LOOP_INTERVAL, _interval));
}

bool	Properties::AppendName(ValueName const& _name)
{
	Delete(TITLE_NAME_NAME);

	return	Append(Property(TITLE_NAME_NAME, _name));
}

bool	Properties::AppendParentID(ValueID const& _parent_id)
{
	return	Append(Property(TITLE_NAME_PARENT_ID, _parent_id));
}

bool	Properties::AppendRegistered(bool _registered)
{
	return	Append(Property(TITLE_NAME_REGISTERED, _registered));
}

bool	Properties::AppendScale(double _scale)
{
	return	Append(Property(TITLE_NAME_SCALE, _scale));
}

bool	Properties::AppendSensorID(std::string const& _sensor_id)
{
	return	Append(Property(TITLE_NAME_SENSOR_ID, _sensor_id));
}

bool	Properties::AppendSNMPModule(std::string const& _module)
{
	return	Append(Property(TITLE_NAME_MODULE, _module));
}

bool	Properties::AppendSNMPCommunity(std::string const& _community)
{
	return	Append(Property(TITLE_NAME_COMMUNITY, _community));
}

bool	Properties::AppendTime(Date const& _date)
{
	Delete(TITLE_NAME_TIME);

	return	Append(Property(TITLE_NAME_TIME, time_t(_date)));
}

bool	Properties::AppendTimeOfExpire(Date const& _time_of_expire)
{
	return	Append(Property(TITLE_NAME_TIME_OF_EXPIRE, time_t(_time_of_expire)));
}

bool	Properties::AppendType(std::string const& _type)
{
	return	Append(Property(TITLE_NAME_TYPE, _type));
}

bool	Properties::AppendUnit(std::string const& _unit)
{
	return	Append(Property(TITLE_NAME_UNIT, _unit));
}

bool	Properties::AppendUpdateInterval(uint32_t _interval)
{
	return	Append(Property(TITLE_NAME_UPDATE_INTERVAL, _interval));
}

bool	Properties::AppendTimeout(uint32_t	_timeout)
{
	return	Append(Property(TITLE_NAME_TIMEOUT, _timeout));
}

bool	Properties::AppendValueMin(ValueFloat const& _value)
{
	return	Append(Property(TITLE_NAME_VALUE_MIN, _value));
}

bool	Properties::AppendValueMax(ValueFloat const& _value)
{
	return	Append(Property(TITLE_NAME_VALUE_MAX, _value));
}

bool	Properties::Delete(std::string const& _name)
{
	const Property*	property = NULL;

	for(auto it = begin(); it != end(); it++)
	{
		if (it->GetName() == _name)
		{
			erase(it);
			return	true;
		}
	}

	return	false;
}

void		Properties::Clear()
{
	clear();
}


uint32_t	Properties::Count()
{
	return	size();
}

const Property*	Properties::Get(std::string const& _name) const
{
	const Property*	property = NULL;

	for(auto it = begin(); it != end(); it++)
	{
		if (it->GetName() == _name)
		{
			property = &(*it);

			break;
		}
	}

	return	property;
}

std::ostream&	operator<<(std::ostream& os, Properties const& _properties)
{	
	JSONNode	json;

	_properties.ToJSON(json);

	return	os << json.write_formatted();
}

void	Properties::ToJSON(JSONNode& _json) const
{
	for(auto it = begin() ; it != end(); it++)
	{
		_json.push_back(JSONNode(*it));	
	}
}

///////////////////////////////////////////////////////////////////////////
// Class	PropertiesList
///////////////////////////////////////////////////////////////////////////

PropertiesList::PropertiesList()
{
}

PropertiesList::PropertiesList(PropertiesList const& _properties_list)
{
	for(auto it = _properties_list.begin() ; it != _properties_list.end() ; it++)
	{
		push_back(*it);
	}
}

PropertiesList::PropertiesList(JSONNode const& _json)
{
	if (_json.type() == JSON_ARRAY)
	{
		for(auto it = _json.begin() ; it != _json.end() ; it++)
		{
			Properties	properties;

			properties.Append(*it);
			push_back(properties);
		}
	}
}

PropertiesList::~PropertiesList()
{
	for(auto it = begin() ; it != end() ; it++)
	{
	}
}

bool	PropertiesList::Append(JSONNode const& _json)
{
	if (_json.type() != JSON_ARRAY)
	{
		return	false;
	}

	for(auto it = _json.begin() ; it != _json.end() ; it++)
	{
		Properties	properties;

		properties.Append(*it);
		push_back(properties);
	}
}

bool	PropertiesList::Append(Properties const& _value)
{
	push_back(_value);

	return	true;
}

PropertiesList::operator JSONNode() const
{
	JSONNode	json(JSON_ARRAY);

	for(auto it = begin() ; it != end() ; it++)
	{
		JSONNode	node;

		it->ToJSON(node);
		json.push_back(node);
	}

	return	json;
}

std::ostream&	operator<<(std::ostream& os, PropertiesList const& _properties_list)
{
	for(auto it = _properties_list.begin(); it != _properties_list.end(); it++)
	{
		os << (*it) << std::endl;
	}
}


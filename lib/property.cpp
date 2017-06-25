#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "defined.h"
#include "property.h"
#include "gateway.h"
#include "device.h"
#include "endpoint.h"
 
uint64_t	PROPERTY_CORRECTION_INTERVAL_FLAG	= ((uint64_t)1) << 0 ;
uint64_t	PROPERTY_DATE_FLAG				= ((uint64_t)1) <<  1;
uint64_t	PROPERTY_OPTIONS_FLAG			= ((uint64_t)1) <<  2;
uint64_t	PROPERTY_ENABLE_FLAG			= ((uint64_t)1) <<  3;	
uint64_t	PROPERTY_ID_FLAG				= ((uint64_t)1) <<  4;
uint64_t	PROPERTY_IP_FLAG				= ((uint64_t)1) <<  5;
uint64_t	PROPERTY_KEEP_ALIVE_INTERVAL_FLAG	= ((uint64_t)1) << 6;
uint64_t	PROPERTY_LOCATION_FLAG			= ((uint64_t)1) <<  7;
uint64_t	PROPERTY_LOOP_INTERVAL_FLAG		= ((uint64_t)1) <<  8;
uint64_t	PROPERTY_NAME_FLAG				= ((uint64_t)1) <<  9;
uint64_t	PROPERTY_PARENT_ID_FLAG			= ((uint64_t)1) <<  10;	
uint64_t	PROPERTY_REGISTERED_FLAG		= ((uint64_t)1) <<  11;
uint64_t	PROPERTY_SCALE_FLAG				= ((uint64_t)1) <<  12;
uint64_t	PROPERTY_SENSOR_ID_FLAG			= ((uint64_t)1) <<  13;
uint64_t	PROPERTY_SNMP_MODULE_FLAG		= ((uint64_t)1) <<  14;
uint64_t	PROPERTY_SNMP_COMMUNITY_FLAG	= ((uint64_t)1) <<  15;
uint64_t	PROPERTY_TIME_FLAG				= ((uint64_t)1) <<  16;
uint64_t	PROPERTY_TIMEOUT_FLAG			= ((uint64_t)1) <<  17;
uint64_t	PROPERTY_TYPE_FLAG				= ((uint64_t)1) <<  18;
uint64_t	PROPERTY_VALUE_MIN_FLAG			= ((uint64_t)1) <<  19;
uint64_t	PROPERTY_VALUE_MAX_FLAG			= ((uint64_t)1) <<  20;
uint64_t	PROPERTY_UNIT_FLAG				= ((uint64_t)1) <<  21;
uint64_t	PROPERTY_UPDATE_INTERVAL_FLAG	= ((uint64_t)1) <<  22;
uint64_t	PROPERTY_TIME_OF_EXPIRE_FLAG	= ((uint64_t)1) <<  23;
uint64_t	PROPERTY_VALUE_FLAG				= ((uint64_t)1) <<  24;
uint64_t	PROPERTY_STAT_FLAG				= ((uint64_t)1) <<  25;

Fields	PROPERTY_CORRECTION_INTERVAL(PROPERTY_CORRECTION_INTERVAL_FLAG);
Fields	PROPERTY_DATE				(PROPERTY_DATE_FLAG);
Fields	PROPERTY_OPTIONS			(PROPERTY_OPTIONS_FLAG);
Fields	PROPERTY_ENABLE				(PROPERTY_ENABLE_FLAG);
Fields	PROPERTY_ID					(PROPERTY_ID_FLAG);
Fields	PROPERTY_IP					(PROPERTY_IP_FLAG);
Fields	PROPERTY_KEEP_ALIVE_INTERVAL(PROPERTY_KEEP_ALIVE_INTERVAL_FLAG);
Fields	PROPERTY_LOCATION			(PROPERTY_LOCATION_FLAG);
Fields	PROPERTY_LOOP_INTERVAL		(PROPERTY_LOOP_INTERVAL_FLAG);
Fields	PROPERTY_NAME				(PROPERTY_NAME_FLAG);
Fields	PROPERTY_PARENT_ID			(PROPERTY_PARENT_ID_FLAG);
Fields	PROPERTY_REGISTERED			(PROPERTY_REGISTERED_FLAG);
Fields	PROPERTY_SCALE				(PROPERTY_SCALE_FLAG);
Fields	PROPERTY_SENSOR_ID			(PROPERTY_SENSOR_ID_FLAG);
Fields	PROPERTY_SNMP_MODULE		(PROPERTY_SNMP_MODULE_FLAG);
Fields	PROPERTY_SNMP_COMMUNITY		(PROPERTY_SNMP_COMMUNITY_FLAG);
Fields	PROPERTY_TIME				(PROPERTY_TIME_FLAG);
Fields	PROPERTY_TIMEOUT			(PROPERTY_TIMEOUT_FLAG);
Fields	PROPERTY_TYPE				(PROPERTY_TYPE_FLAG);
Fields	PROPERTY_VALUE_MIN			(PROPERTY_VALUE_MIN_FLAG);
Fields	PROPERTY_VALUE_MAX			(PROPERTY_VALUE_MAX_FLAG);
Fields	PROPERTY_UNIT				(PROPERTY_UNIT_FLAG);
Fields	PROPERTY_UPDATE_INTERVAL	(PROPERTY_UPDATE_INTERVAL_FLAG);
Fields	PROPERTY_TIME_OF_EXPIRE		(PROPERTY_TIME_OF_EXPIRE_FLAG);
Fields	PROPERTY_VALUE				(PROPERTY_VALUE_FLAG);
Fields	PROPERTY_STAT				(PROPERTY_STAT_FLAG);

Fields	PROPERTY_ALL				( true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false, false, false);
Fields	PROPERTY_CREATE				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false, false, false);
Fields	PROPERTY_ADD_CONFIRM		( true, false,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false, false, false);
Fields	PROPERTY_GET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false, false, false);
Fields	PROPERTY_SET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false, false, false);

Fields::Fields()
{
	correction_interval = false;
	date = false;
	options = false;
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
	value = false;
	stat = false;
}

Fields::Fields( bool	_correction_interval, bool	_date, bool	_options, bool	_enable, bool	_id, bool	_ip, bool	_keep_alive_interval, bool	_location, bool	_loop_interval, bool	_name,
				bool	_parent_id, bool	_registered, bool	_scale, bool	_sensor_id, bool	_snmp_module, bool	_snmp_community, bool	_time, bool	_timeout, bool	_type, bool	_value_min,
				bool	_value_max, bool	_unit, bool	_update_interval, bool _time_of_expire, bool _value, bool _stat)
:correction_interval(_correction_interval), date(date), options(_options), enable(_enable), id(_id), ip(_ip), keep_alive_interval(_keep_alive_interval), location(_location), loop_interval(_loop_interval), name(_name),
				parent_id(_parent_id), registered(_registered), scale(_scale), sensor_id(_sensor_id), snmp_module(_snmp_module), snmp_community(_snmp_community), time(_time), timeout(_timeout), type(_type),
				value_min(_value_min), value_max(_value_max), unit(_unit), update_interval(_update_interval), time_of_expire(_time_of_expire), value(_value)
{
}

Fields::Fields(uint64_t _flags)
{
	Set(_flags);
}

Fields	operator+(Fields& _field1, Fields& _fields2)
{
	Fields	fields;

	fields.correction_interval = _field1.correction_interval || _fields2.correction_interval;
	fields.date = _field1.date || _fields2.date;
	fields.options= _field1.options || _fields2.options;
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
	fields.value = _field1.value || _fields2.value;
	fields.stat = _field1.stat || _fields2.stat;

	return	fields;
}

Fields&	Fields::operator+=(Fields& _fields)
{
	correction_interval = correction_interval || _fields.correction_interval;
	date = date || _fields.date;
	options = options || _fields.options;
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
	value = value|| _fields.value;
	stat = stat || _fields.stat;

	return	*this;
}

bool	Fields::Set(uint64_t _value)
{
	if (_value & PROPERTY_CORRECTION_INTERVAL_FLAG)
	{
		correction_interval = true;
	}
	if (_value & PROPERTY_DATE_FLAG)
	{
		date = true;
	}
	if (_value & PROPERTY_OPTIONS_FLAG)
	{
		options = true;
	}
	if (_value & PROPERTY_ENABLE_FLAG)
	{
		enable = true;
	}
	if (_value & PROPERTY_ID_FLAG)
	{
		id = true;
	}
	if (_value & PROPERTY_IP_FLAG)
	{
		ip = true;
	}
	if (_value & PROPERTY_KEEP_ALIVE_INTERVAL_FLAG)
	{
		keep_alive_interval = true;
	}
	if (_value & PROPERTY_LOCATION_FLAG)
	{
		location = true;
	}
	if (_value & PROPERTY_LOOP_INTERVAL_FLAG)
	{
		loop_interval = true;
	}
	if (_value & PROPERTY_NAME_FLAG)
	{
		name = true;
	}
	if (_value & PROPERTY_PARENT_ID_FLAG)
	{
		parent_id = true;
	}
	if (_value & PROPERTY_REGISTERED_FLAG)
	{
		registered = true;
	}
	if (_value & PROPERTY_SCALE_FLAG)
	{
		scale = true;
	}
	if (_value & PROPERTY_SENSOR_ID_FLAG)
	{
		sensor_id = true;
	}
	if (_value & PROPERTY_SNMP_MODULE_FLAG)
	{
		snmp_module = true;
	}
	if (_value & PROPERTY_SNMP_COMMUNITY_FLAG)
	{
		snmp_community = true;
	}
	if (_value & PROPERTY_TIME_FLAG)
	{
		time = true;
	}
	if (_value & PROPERTY_TIMEOUT_FLAG)
	{
		timeout = true;
	}
	if (_value & PROPERTY_TYPE_FLAG)
	{
		type = true;
	}
	if (_value & PROPERTY_VALUE_MIN_FLAG)
	{
		value_min = true;
	}
	if (_value & PROPERTY_VALUE_MAX_FLAG)
	{
		value_max = true;
	}
	if (_value & PROPERTY_UNIT_FLAG)
	{
		unit = true;
	}
	if (_value & PROPERTY_UPDATE_INTERVAL_FLAG)
	{
		update_interval = true;
	}
	if (_value & PROPERTY_TIME_OF_EXPIRE_FLAG)
	{
		time_of_expire = true;
	}
	if (_value & PROPERTY_VALUE_FLAG)
	{
		value = true;
	}
	if (_value & PROPERTY_STAT_FLAG)
	{
		stat = true;
	}

	return	true;
}

bool	Fields::Reset(uint64_t _value)
{
	if (_value & PROPERTY_CORRECTION_INTERVAL_FLAG)
	{
		correction_interval = false;
	}
	if (_value & PROPERTY_DATE_FLAG)
	{
		date = false;
	}
	if (_value & PROPERTY_OPTIONS_FLAG)
	{
		options = false;
	}
	if (_value & PROPERTY_ENABLE_FLAG)
	{
		enable = false;
	}
	if (_value & PROPERTY_ID_FLAG)
	{
		id = false;
	}
	if (_value & PROPERTY_IP_FLAG)
	{
		ip = false;
	}
	if (_value & PROPERTY_KEEP_ALIVE_INTERVAL_FLAG)
	{
		keep_alive_interval = false;
	}
	if (_value & PROPERTY_LOCATION_FLAG)
	{
		location = false;
	}
	if (_value & PROPERTY_LOOP_INTERVAL_FLAG)
	{
		loop_interval = false;
	}
	if (_value & PROPERTY_NAME_FLAG)
	{
		name = false;
	}
	if (_value & PROPERTY_PARENT_ID_FLAG)
	{
		parent_id = false;
	}
	if (_value & PROPERTY_REGISTERED_FLAG)
	{
		registered = false;
	}
	if (_value & PROPERTY_SCALE_FLAG)
	{
		scale = false;
	}
	if (_value & PROPERTY_SENSOR_ID_FLAG)
	{
		sensor_id = false;
	}
	if (_value & PROPERTY_SNMP_MODULE_FLAG)
	{
		snmp_module = false;
	}
	if (_value & PROPERTY_SNMP_COMMUNITY_FLAG)
	{
		snmp_community = false;
	}
	if (_value & PROPERTY_TIME_FLAG)
	{
		time = false;
	}
	if (_value & PROPERTY_TIMEOUT_FLAG)
	{
		timeout = false;
	}
	if (_value & PROPERTY_TYPE_FLAG)
	{
		type = false;
	}
	if (_value & PROPERTY_VALUE_MIN_FLAG)
	{
		value_min = false;
	}
	if (_value & PROPERTY_VALUE_MAX_FLAG)
	{
		value_max = false;
	}
	if (_value & PROPERTY_UNIT_FLAG)
	{
		unit = false;
	}
	if (_value & PROPERTY_UPDATE_INTERVAL_FLAG)
	{
		update_interval = false;
	}
	if (_value & PROPERTY_TIME_OF_EXPIRE_FLAG)
	{
		time_of_expire = false;
	}
	if (_value & PROPERTY_VALUE_FLAG)
	{
		value = false;
	}
	if (_value & PROPERTY_STAT_FLAG)
	{
		stat = false;
	}

	return	true;
}

bool	Fields::Set(std::string const& _field)
{
	if (_field == TITLE_NAME_CORRECTION_INTERVAL)
	{
		correction_interval = true;
	}
	else if (_field == TITLE_NAME_DATE)
	{
		date = true;
	}
	else if (_field == TITLE_NAME_OPTIONS)
	{
		options = true;
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
	else if (_field == TITLE_NAME_VALUE)
	{
		value = true;
	}
	else if (_field == TITLE_NAME_STAT)
	{
		stat = true;
	}
	else
	{
		return	false;	
	}

	return	true;
}


bool	Fields::Reset(std::string const& _field)
{
	if (_field == TITLE_NAME_CORRECTION_INTERVAL)
	{
		correction_interval = false;
	}
	else if (_field == TITLE_NAME_DATE)
	{
		date = false;
	}
	else if (_field == TITLE_NAME_OPTIONS)
	{
		options = false;
	}
	else if (_field == TITLE_NAME_ENABLE)
	{
		enable = false;
	}
	else if (_field == TITLE_NAME_ID)
	{
		id = false;
	}
	else if (_field == TITLE_NAME_IP)
	{
		ip = false;
	}
	else if (_field == TITLE_NAME_KEEP_ALIVE_INTERVAL)
	{
		keep_alive_interval = false;
	}
	else if (_field == TITLE_NAME_LOCATION)
	{
		location = false;
	}
	else if (_field == TITLE_NAME_LOOP_INTERVAL)
	{
		loop_interval = false;
	}
	else if (_field == TITLE_NAME_NAME)
	{
		name = false;
	}
	else if (_field == TITLE_NAME_PARENT_ID)
	{
		parent_id = false;
	}
	else if (_field == TITLE_NAME_REGISTERED)
	{
		registered = false;
	}
	if (_field == TITLE_NAME_SCALE)
	{
		scale = false;
	}
	if (_field == TITLE_NAME_SENSOR_ID)
	{
		sensor_id = false;
	}
	if (_field == TITLE_NAME_MODULE)
	{
		snmp_module = false;
	}
	if (_field == TITLE_NAME_COMMUNITY)
	{
		snmp_community = false;
	}
	if (_field == TITLE_NAME_TIME)
	{
		time = false;
	}
	if (_field == TITLE_NAME_TIMEOUT)
	{
		timeout = false;
	}
	if (_field == TITLE_NAME_TYPE)
	{
		type = false;
	}
	if (_field == TITLE_NAME_VALUE_MIN)
	{
		value_min = false;
	}
	if (_field == TITLE_NAME_VALUE_MAX)
	{
		value_max = false;
	}
	if (_field == TITLE_NAME_UNIT)
	{
		unit = false;
	}
	if (_field == TITLE_NAME_UPDATE_INTERVAL)
	{
		update_interval = false;
	}
	if (_field == TITLE_NAME_TIME_OF_EXPIRE)
	{
		time_of_expire = false;
	}
	if (_field == TITLE_NAME_VALUE)
	{
		value = false;
	}
	if (_field == TITLE_NAME_STAT)
	{
		stat = false;
	}
	else
	{
		return	false;	
	}

	return	true;
}


Fields::operator uint64_t()
{
	uint64_t	value = 0;

	if (correction_interval)
	{
		value |= PROPERTY_CORRECTION_INTERVAL_FLAG;
	}
	if (date)
	{
		value |= PROPERTY_DATE_FLAG;
	}
	if (options)
	{
		value |= PROPERTY_OPTIONS_FLAG;
	}
	if (enable)
	{
		value |= PROPERTY_ENABLE_FLAG;
	}
	if (id)
	{
		value |= PROPERTY_ID_FLAG;
	}
	if (ip)
	{
		value |= PROPERTY_IP_FLAG;
	}
	if (keep_alive_interval)
	{
		value |= PROPERTY_KEEP_ALIVE_INTERVAL_FLAG;
	}
	if (location)
	{
		value |= PROPERTY_LOCATION_FLAG;
	}
	if (loop_interval)
	{
		value |= PROPERTY_LOOP_INTERVAL_FLAG;
	}
	if (name)
	{
		value |= PROPERTY_NAME_FLAG;
	}
	if (parent_id)
	{
		value |= PROPERTY_PARENT_ID_FLAG;
	}
	if (registered)
	{
		value |= PROPERTY_REGISTERED_FLAG;
	}
	if (scale)
	{
		value |= PROPERTY_SCALE_FLAG;
	}
	if (sensor_id)
	{
		value |= PROPERTY_SENSOR_ID_FLAG;
	}
	if (snmp_module)
	{
		value |= PROPERTY_SNMP_MODULE_FLAG;
	}
	if (snmp_community)
	{
		value |=PROPERTY_SNMP_COMMUNITY_FLAG;
	}
	if (time)
	{
		value |= PROPERTY_TIME_FLAG;
	}
	if (timeout)
	{
		value |= PROPERTY_TIMEOUT_FLAG;
	}
	if (type)
	{
		value |= PROPERTY_TYPE_FLAG;
	}
	if (value_min)
	{
		value |= PROPERTY_VALUE_MIN_FLAG;
	}
	if (value_max)
	{
		value |= PROPERTY_VALUE_MAX_FLAG;
	}
	if (unit)
	{
		value |= PROPERTY_UNIT_FLAG;
	}
	if (update_interval)
	{
		value |= PROPERTY_UPDATE_INTERVAL_FLAG;
	}
	if (time_of_expire)
	{
		value |= PROPERTY_TIME_OF_EXPIRE_FLAG;
	}
	if (value)
	{
		value |= PROPERTY_VALUE_FLAG;
	}
	if (stat)
	{
		stat |= PROPERTY_VALUE_FLAG;
	}

	return	value;
}


bool	Fields::Names(std::multimap<uint64_t, std::string>& _names)
{
	if (correction_interval)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_CORRECTION_INTERVAL_FLAG, TITLE_NAME_CORRECTION_INTERVAL));
	}

	if (date)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_DATE_FLAG, TITLE_NAME_DATE));
	}

	if (options)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_OPTIONS_FLAG, TITLE_NAME_OPTIONS));
	}

	if (enable)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_ENABLE_FLAG, TITLE_NAME_ENABLE));
	}

	if (id)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_ID_FLAG, TITLE_NAME_ID));
	}

	if (ip)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_IP_FLAG, TITLE_NAME_IP));
	}

	if (keep_alive_interval)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_KEEP_ALIVE_INTERVAL_FLAG, TITLE_NAME_KEEP_ALIVE_INTERVAL));
	}

	if (location)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_LOCATION_FLAG, TITLE_NAME_LOCATION));
	}

	if (loop_interval)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_LOOP_INTERVAL_FLAG, TITLE_NAME_LOOP_INTERVAL));
	}

	if (name)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_NAME_FLAG, TITLE_NAME_NAME));
	}

	if (parent_id)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_PARENT_ID_FLAG, TITLE_NAME_PARENT_ID));
	}
	
	if (registered)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_REGISTERED_FLAG, TITLE_NAME_REGISTERED));
	}
	
	if (scale)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_SCALE_FLAG, TITLE_NAME_SCALE));
	}
	
	if (sensor_id)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_SENSOR_ID_FLAG, TITLE_NAME_SENSOR_ID));
	}
	
	if (snmp_module)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_SNMP_MODULE_FLAG, TITLE_NAME_MODULE));
	}
	
	if (snmp_community)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_SNMP_COMMUNITY_FLAG, TITLE_NAME_COMMUNITY));
	}
	
	if (time)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_TIME_FLAG, TITLE_NAME_TIME));
	}
	
	if (timeout)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_TIMEOUT_FLAG, TITLE_NAME_TIMEOUT));
	}
	
	if (type)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_TYPE_FLAG, TITLE_NAME_TYPE));
	}
	
	if (value_min)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_VALUE_MIN_FLAG, TITLE_NAME_VALUE_MIN));
	}
	
	if (value_max)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_VALUE_MAX_FLAG, TITLE_NAME_VALUE_MAX));
	}
	
	if (unit)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_UNIT_FLAG, TITLE_NAME_UNIT));
	}
	
	if (update_interval)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_UPDATE_INTERVAL_FLAG, TITLE_NAME_UPDATE_INTERVAL));
	}
	
	if (time_of_expire)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_TIME_OF_EXPIRE_FLAG, TITLE_NAME_TIME_OF_EXPIRE));
	}
	
	if (value)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_VALUE_FLAG, TITLE_NAME_VALUE));
	}
	
	if (stat)
	{
		_names.insert(_names.end(), std::pair<uint64_t, std::string>(PROPERTY_STAT_FLAG, TITLE_NAME_STAT));
	}

	return	true;

}

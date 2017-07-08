#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "defined.h"
#include "property.h"
#include "gateway.h"
#include "device.h"
#include "endpoint.h"
 
Fields	PROPERTY_ID					(PROPERTY_ID_FLAG);
Fields	PROPERTY_TYPE				(PROPERTY_TYPE_FLAG);
Fields	PROPERTY_MODEL				(PROPERTY_MODEL_FLAG);
Fields	PROPERTY_NAME				(PROPERTY_NAME_FLAG);
Fields	PROPERTY_TIME				(PROPERTY_TIME_FLAG);
Fields	PROPERTY_ENABLE				(PROPERTY_ENABLE_FLAG);
Fields	PROPERTY_STAT				(PROPERTY_STAT_FLAG);
Fields	PROPERTY_LOCATION			(PROPERTY_LOCATION_FLAG);
Fields	PROPERTY_REGISTERED			(PROPERTY_REGISTERED_FLAG);
Fields	PROPERTY_PARENT_ID			(PROPERTY_PARENT_ID_FLAG);

Fields	PROPERTY_IP					(PROPERTY_IP_FLAG);
Fields	PROPERTY_SENSOR_ID			(PROPERTY_SENSOR_ID_FLAG);
Fields	PROPERTY_UNIT				(PROPERTY_UNIT_FLAG);
Fields	PROPERTY_SCALE				(PROPERTY_SCALE_FLAG);
Fields	PROPERTY_VALUE_MIN			(PROPERTY_VALUE_MIN_FLAG);
Fields	PROPERTY_VALUE_MAX			(PROPERTY_VALUE_MAX_FLAG);

Fields	PROPERTY_OPTIONS			(PROPERTY_OPTIONS_FLAG);


Fields	PROPERTY_SNMP_MODULE		(PROPERTY_SNMP_MODULE_FLAG);
Fields	PROPERTY_SNMP_COMMUNITY		(PROPERTY_SNMP_COMMUNITY_FLAG);
Fields	PROPERTY_TIMEOUT			(PROPERTY_TIMEOUT_FLAG);

Fields	PROPERTY_CORRECTION_INTERVAL(PROPERTY_CORRECTION_INTERVAL_FLAG);
Fields	PROPERTY_KEEP_ALIVE_INTERVAL(PROPERTY_KEEP_ALIVE_INTERVAL_FLAG);
Fields	PROPERTY_LOOP_INTERVAL		(PROPERTY_LOOP_INTERVAL_FLAG);

Fields	PROPERTY_UPDATE_INTERVAL	(PROPERTY_UPDATE_INTERVAL_FLAG);
Fields	PROPERTY_TIME_OF_EXPIRE		(PROPERTY_TIME_OF_EXPIRE_FLAG);
Fields	PROPERTY_VALUE				(PROPERTY_VALUE_FLAG);

Fields	PROPERTY_BASIC	= PROPERTY_ID
						+ PROPERTY_TYPE
						+ PROPERTY_NAME
						+ PROPERTY_MODEL
						+ PROPERTY_TIME
						+ PROPERTY_ENABLE
						+ PROPERTY_LOCATION
						+ PROPERTY_REGISTERED
						+ PROPERTY_PARENT_ID
						+ PROPERTY_IP				
						+ PROPERTY_SENSOR_ID
						+ PROPERTY_UNIT
						+ PROPERTY_SCALE	
						+ PROPERTY_VALUE_MIN
						+ PROPERTY_VALUE_MAX
						+ PROPERTY_OPTIONS
						+ PROPERTY_SNMP_MODULE
						+ PROPERTY_SNMP_COMMUNITY
						+ PROPERTY_TIMEOUT
						+ PROPERTY_KEEP_ALIVE_INTERVAL
						+ PROPERTY_CORRECTION_INTERVAL
						+ PROPERTY_LOOP_INTERVAL;


//const Fields	PROPERTY_ALL	= PROPERTY_BASIC;

Fields::Fields()
{
	id = false;
	type = false;
	name = false;
	model= false;
	enable = false;
	stat = false;
	location = false;
	registered = false;
	parent_id = false;
	ip = false;
	sensor_id = false;
	unit = false;
	scale = false;
	value_min = false;
	value_max = false;
	options = false;
	time = false;
	snmp_module = false;
	snmp_community = false;
	timeout = false;
	correction_interval = false;
	keep_alive_interval = false;
	loop_interval = false;
	update_interval = false;
	time_of_expire = false;
	value = false;
}

Fields::Fields(uint32_t _flags)
{
	Set(_flags);
}

Fields	operator+(Fields& _fields1, Fields& _fields2)
{
	uint32_t	flags;
	uint32_t	flags1, flags2;

	flags1 = uint32_t(_fields1);
	flags2 = uint32_t(_fields2);

	flags = flags1 | flags2;

	Fields	fields(flags);

	return	fields;
}

Fields&	Fields::operator+=(Fields& _fields2)
{
	uint32_t	flags;
	uint32_t	flags1;
	uint32_t	flags2;

	flags1 = uint32_t(*this);
	flags2 = uint32_t(_fields2);

	flags = flags1 | flags2;

	Set(flags);

	return	*this;
}

bool	Fields::Set(uint32_t _value)
{
	if (_value & PROPERTY_CORRECTION_INTERVAL_FLAG)
	{
		correction_interval = true;
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
	if (_value & PROPERTY_MODEL_FLAG)
	{
		model = true;
	}

	return	true;
}

bool	Fields::Reset(uint32_t _value)
{
	if (_value & PROPERTY_CORRECTION_INTERVAL_FLAG)
	{
		correction_interval = false;
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
	if (_value & PROPERTY_MODEL_FLAG)
	{
		model = false;
	}

	return	true;
}

bool	Fields::Set(std::string const& _field)
{
	if (_field == TITLE_NAME_CORRECTION_INTERVAL)
	{
		correction_interval = true;
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
	else if (_field == TITLE_NAME_MODEL)
	{
		model = true;
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
	if (_field == TITLE_NAME_MODEL)
	{
		model = false;
	}
	else
	{
		return	false;	
	}

	return	true;
}


Fields::operator uint32_t() const
{
	uint32_t	value = 0;

	if (correction_interval)
	{
		value |= PROPERTY_CORRECTION_INTERVAL_FLAG;
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
		value |= PROPERTY_STAT_FLAG;
	}
	if (model)
	{
		value |= PROPERTY_MODEL_FLAG;
	}

	return	value;
}


bool	Fields::Names(std::multimap<uint32_t, std::string>& _names)
{
	if (correction_interval)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_CORRECTION_INTERVAL_FLAG, TITLE_NAME_CORRECTION_INTERVAL));
	}

	if (options)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_OPTIONS_FLAG, TITLE_NAME_OPTIONS));
	}

	if (enable)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_ENABLE_FLAG, TITLE_NAME_ENABLE));
	}

	if (id)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_ID_FLAG, TITLE_NAME_ID));
	}

	if (ip)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_IP_FLAG, TITLE_NAME_IP));
	}

	if (keep_alive_interval)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_KEEP_ALIVE_INTERVAL_FLAG, TITLE_NAME_KEEP_ALIVE_INTERVAL));
	}

	if (location)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_LOCATION_FLAG, TITLE_NAME_LOCATION));
	}

	if (loop_interval)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_LOOP_INTERVAL_FLAG, TITLE_NAME_LOOP_INTERVAL));
	}

	if (name)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_NAME_FLAG, TITLE_NAME_NAME));
	}

	if (parent_id)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_PARENT_ID_FLAG, TITLE_NAME_PARENT_ID));
	}
	
	if (registered)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_REGISTERED_FLAG, TITLE_NAME_REGISTERED));
	}
	
	if (scale)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_SCALE_FLAG, TITLE_NAME_SCALE));
	}
	
	if (sensor_id)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_SENSOR_ID_FLAG, TITLE_NAME_SENSOR_ID));
	}
	
	if (snmp_module)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_SNMP_MODULE_FLAG, TITLE_NAME_MODULE));
	}
	
	if (snmp_community)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_SNMP_COMMUNITY_FLAG, TITLE_NAME_COMMUNITY));
	}
	
	if (time)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_TIME_FLAG, TITLE_NAME_TIME));
	}
	
	if (timeout)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_TIMEOUT_FLAG, TITLE_NAME_TIMEOUT));
	}
	
	if (type)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_TYPE_FLAG, TITLE_NAME_TYPE));
	}
	
	if (value_min)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_VALUE_MIN_FLAG, TITLE_NAME_VALUE_MIN));
	}
	
	if (value_max)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_VALUE_MAX_FLAG, TITLE_NAME_VALUE_MAX));
	}
	
	if (unit)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_UNIT_FLAG, TITLE_NAME_UNIT));
	}
	
	if (update_interval)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_UPDATE_INTERVAL_FLAG, TITLE_NAME_UPDATE_INTERVAL));
	}
	
	if (time_of_expire)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_TIME_OF_EXPIRE_FLAG, TITLE_NAME_TIME_OF_EXPIRE));
	}
	
	if (value)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_VALUE_FLAG, TITLE_NAME_VALUE));
	}
	
	if (stat)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_STAT_FLAG, TITLE_NAME_STAT));
	}

	if (model)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_MODEL_FLAG, TITLE_NAME_MODEL));
	}

	return	true;

}

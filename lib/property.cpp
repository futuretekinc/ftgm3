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
Fields	PROPERTY_DB_REMOVE			(PROPERTY_DB_REMOVE_FLAG);
Fields	PROPERTY_DEV_ID				(PROPERTY_DEV_ID_FLAG);

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
						+ PROPERTY_LOOP_INTERVAL
						+ PROPERTY_DEV_ID;


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
	db_remove = false;
	dev_id = false;
}

Fields::Fields(uint32_t _flags)
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
	db_remove = false;
	dev_id = false;
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

std::ostream& operator <<(std::ostream& os, Fields const& _fields)
{
	os << std::setw(8) << std::setfill('0')<< std::setbase(16) << uint32_t(_fields) << std::setfill(' ') << std::setw(0) << std::setbase(10);

	return	os;
}

bool	Fields::Set(uint32_t _value)
{
	//#define	PROPERTY_ID_FLAG					(((uint32_t)1) <<  0)
	if (_value & PROPERTY_ID_FLAG)
	{
		id = true;
	}
	//#define	PROPERTY_TYPE_FLAG					(((uint32_t)1) <<  1)
	if (_value & PROPERTY_TYPE_FLAG)
	{
		type = true;
	}
	//#define	PROPERTY_MODEL_FLAG					(((uint32_t)1) <<  2)
	if (_value & PROPERTY_MODEL_FLAG)
	{
		model = true;
	}
	//#define	PROPERTY_NAME_FLAG					(((uint32_t)1) <<  3)
	if (_value & PROPERTY_NAME_FLAG)
	{
		name = true;
	}
	//#define	PROPERTY_TIME_FLAG					(((uint32_t)1) <<  4)
	if (_value & PROPERTY_TIME_FLAG)
	{
		time = true;
	}
	//#define	PROPERTY_ENABLE_FLAG				(((uint32_t)1) <<  5)	
	if (_value & PROPERTY_ENABLE_FLAG)
	{
		enable = true;
	}
	//#define	PROPERTY_STAT_FLAG					(((uint32_t)1) <<  6)
	if (_value & PROPERTY_STAT_FLAG)
	{
		stat = true;
	}
	//#define	PROPERTY_LOCATION_FLAG				(((uint32_t)1) <<  7)
	if (_value & PROPERTY_LOCATION_FLAG)
	{
		location = true;
	}
	//#define	PROPERTY_REGISTERED_FLAG			(((uint32_t)1) <<  8)
	if (_value & PROPERTY_REGISTERED_FLAG)
	{
		registered = true;
	}
	//#define	PROPERTY_PARENT_ID_FLAG				(((uint32_t)1) <<  9)	
	if (_value & PROPERTY_PARENT_ID_FLAG)
	{
		parent_id = true;
	}
	
	//#define	PROPERTY_IP_FLAG					(((uint32_t)1) <<  10)
	if (_value & PROPERTY_IP_FLAG)
	{
		ip = true;
	}
	//#define	PROPERTY_SENSOR_ID_FLAG				(((uint32_t)1) <<  11)
	if (_value & PROPERTY_SENSOR_ID_FLAG)
	{
		sensor_id = true;
	}
	//#define	PROPERTY_UNIT_FLAG					(((uint32_t)1) <<  12)
	if (_value & PROPERTY_UNIT_FLAG)
	{
		unit = true;
	}
	//#define	PROPERTY_SCALE_FLAG					(((uint32_t)1) <<  13)
	if (_value & PROPERTY_SCALE_FLAG)
	{
		scale = true;
	}
	//#define	PROPERTY_VALUE_MIN_FLAG				(((uint32_t)1) <<  14)
	if (_value & PROPERTY_VALUE_MIN_FLAG)
	{
		value_min = true;
	}
	//#define	PROPERTY_VALUE_MAX_FLAG				(((uint32_t)1) <<  15)
	if (_value & PROPERTY_VALUE_MAX_FLAG)
	{
		value_max = true;
	}
	//
	//#define	PROPERTY_OPTIONS_FLAG				(((uint32_t)1) <<  16)
	if (_value & PROPERTY_OPTIONS_FLAG)
	{
		options = true;
	}
	
	//#define	PROPERTY_SNMP_MODULE_FLAG			(((uint32_t)1) <<  17)
	if (_value & PROPERTY_SNMP_MODULE_FLAG)
	{
		snmp_module = true;
	}
	//#define	PROPERTY_SNMP_COMMUNITY_FLAG		(((uint32_t)1) <<  18)
	if (_value & PROPERTY_SNMP_COMMUNITY_FLAG)
	{
		snmp_community = true;
	}
	//#define	PROPERTY_TIMEOUT_FLAG				(((uint32_t)1) <<  19)
	if (_value & PROPERTY_TIMEOUT_FLAG)
	{
		timeout = true;
	}
	
	//#define	PROPERTY_CORRECTION_INTERVAL_FLAG	(((uint32_t)1) <<  20)
	if (_value & PROPERTY_CORRECTION_INTERVAL_FLAG)
	{
		correction_interval = true;
	}
	//#define	PROPERTY_KEEP_ALIVE_INTERVAL_FLAG	(((uint32_t)1) <<  21)
	if (_value & PROPERTY_KEEP_ALIVE_INTERVAL_FLAG)
	{
		keep_alive_interval = true;
	}
	//#define	PROPERTY_LOOP_INTERVAL_FLAG			(((uint32_t)1) <<  22)
	if (_value & PROPERTY_LOOP_INTERVAL_FLAG)
	{
		loop_interval = true;
	}
	//#define	PROPERTY_UPDATE_INTERVAL_FLAG		(((uint32_t)1) <<  23)
	if (_value & PROPERTY_UPDATE_INTERVAL_FLAG)
	{
		update_interval = true;
	}
	//#define	PROPERTY_TIME_OF_EXPIRE_FLAG		(((uint32_t)1) <<  24)
	if (_value & PROPERTY_TIME_OF_EXPIRE_FLAG)
	{
		time_of_expire = true;
	}
	//#define	PROPERTY_VALUE_FLAG					(((uint32_t)1) <<  25)
	if (_value & PROPERTY_VALUE_FLAG)
	{
		value = true;
	}

	//#define	PROPERTY_DB_REMOVE_FLAG
	if(_value & PROPERTY_DB_REMOVE_FLAG)
	{
		db_remove = true;
	}
	
	if(_value & PROPERTY_DEV_ID_FLAG)
	{
		dev_id = true;
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
	if(_value & PROPERTY_DB_REMOVE_FLAG)
	{
		db_remove = false;
	}
	if(_value & PROPERTY_DEV_ID_FLAG)
	{
		dev_id = false;
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
	uint32_t	flags = 0;

	//	PROPERTY_ID_FLAG					(((uint32_t)1) <<  0)
	if (id)
	{
		flags |= PROPERTY_ID_FLAG;
	}
	//	PROPERTY_TYPE_FLAG					(((uint32_t)1) <<  1)
	if (type)
	{
		flags |= PROPERTY_TYPE_FLAG;
	}
	//	PROPERTY_MODEL_FLAG					(((uint32_t)1) <<  2)
	if (model)
	{
		flags |= PROPERTY_MODEL_FLAG;
	}
	//	PROPERTY_NAME_FLAG					(((uint32_t)1) <<  3)
	if (name)
	{
		flags |= PROPERTY_NAME_FLAG;
	}
	//	PROPERTY_TIME_FLAG					(((uint32_t)1) <<  4)
	if (time)
	{
		flags |= PROPERTY_TIME_FLAG;
	}
	//	PROPERTY_ENABLE_FLAG				(((uint32_t)1) <<  5)	
	if (enable)
	{
		flags |= PROPERTY_ENABLE_FLAG;
	}
	//	PROPERTY_STAT_FLAG					(((uint32_t)1) <<  6)
	if (stat)
	{
		flags |= PROPERTY_STAT_FLAG;
	}
	//	PROPERTY_LOCATION_FLAG				(((uint32_t)1) <<  7)
	if (location)
	{
		flags |= PROPERTY_LOCATION_FLAG;
	}
	//	PROPERTY_REGISTERED_FLAG			(((uint32_t)1) <<  8)
	if (registered)
	{
		flags |= PROPERTY_REGISTERED_FLAG;
	}
	//	PROPERTY_PARENT_ID_FLAG				(((uint32_t)1) <<  9)	
	if (parent_id)
	{
		flags |= PROPERTY_PARENT_ID_FLAG;
	}

	//	PROPERTY_IP_FLAG					(((uint32_t)1) <<  10)
	if (ip)
	{
		flags |= PROPERTY_IP_FLAG;
	}
	//	PROPERTY_SENSOR_ID_FLAG				(((uint32_t)1) <<  11)
	if (sensor_id)
	{
		flags |= PROPERTY_SENSOR_ID_FLAG;
	}
	//	PROPERTY_UNIT_FLAG					(((uint32_t)1) <<  12)
	if (unit)
	{
		flags |= PROPERTY_UNIT_FLAG;
	}
	//	PROPERTY_SCALE_FLAG					(((uint32_t)1) <<  13)
	if (scale)
	{
		flags |= PROPERTY_SCALE_FLAG;
	}
	//	PROPERTY_VALUE_MIN_FLAG				(((uint32_t)1) <<  14)
	if (value_min)
	{
		flags |= PROPERTY_VALUE_MIN_FLAG;
	}
	//	PROPERTY_VALUE_MAX_FLAG				(((uint32_t)1) <<  15)
	if (value_max)
	{
		flags |= PROPERTY_VALUE_MAX_FLAG;
	}

	//	PROPERTY_OPTIONS_FLAG				(((uint32_t)1) <<  16)
	if (options)
	{
		flags |= PROPERTY_OPTIONS_FLAG;
	}

	//	PROPERTY_SNMP_MODULE_FLAG			(((uint32_t)1) <<  17)
	if (snmp_module)
	{
		flags |= PROPERTY_SNMP_MODULE_FLAG;
	}
	//	PROPERTY_SNMP_COMMUNITY_FLAG		(((uint32_t)1) <<  18)
	if (snmp_community)
	{
		flags |=PROPERTY_SNMP_COMMUNITY_FLAG;
	}
	//	PROPERTY_TIMEOUT_FLAG				(((uint32_t)1) <<  19)
	if (timeout)
	{
		flags |= PROPERTY_TIMEOUT_FLAG;
	}

	//	PROPERTY_CORRECTION_INTERVAL_FLAG	(((uint32_t)1) <<  20)
	if (correction_interval)
	{
		flags |= PROPERTY_CORRECTION_INTERVAL_FLAG;
	}
	//	PROPERTY_KEEP_ALIVE_INTERVAL_FLAG	(((uint32_t)1) <<  21)
	if (keep_alive_interval)
	{
		flags |= PROPERTY_KEEP_ALIVE_INTERVAL_FLAG;
	}
	//	PROPERTY_LOOP_INTERVAL_FLAG			(((uint32_t)1) <<  22)
	if (loop_interval)
	{
		flags |= PROPERTY_LOOP_INTERVAL_FLAG;
	}
	//	PROPERTY_UPDATE_INTERVAL_FLAG		(((uint32_t)1) <<  23)
	if (update_interval)
	{
		flags |= PROPERTY_UPDATE_INTERVAL_FLAG;
	}
	//	PROPERTY_TIME_OF_EXPIRE_FLAG		(((uint32_t)1) <<  24)
	if (time_of_expire)
	{
		flags |= PROPERTY_TIME_OF_EXPIRE_FLAG;
	}
	//	PROPERTY_VALUE_FLAG					(((uint32_t)1) <<  25)
	if (value)
	{
		flags |= PROPERTY_VALUE_FLAG;
	}

	if(db_remove)
	{
		flags |= PROPERTY_DB_REMOVE_FLAG;
	}
	if(dev_id)
	{
		flags |= PROPERTY_DEV_ID_FLAG;
	}
	return	flags;
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
	if (db_remove)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_DB_REMOVE_FLAG, TITLE_NAME_DB_REMOVE));
	}
	if (dev_id)
	{
		_names.insert(_names.end(), std::pair<uint32_t, std::string>(PROPERTY_DEV_ID_FLAG, TITLE_NAME_DEV_ID));
	}
	return	true;

}

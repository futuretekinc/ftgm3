#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "defined.h"
#include "property.h"
#include "gateway.h"
#include "device.h"
#include "endpoint.h"
 
Fields	PROPERTY_CORRECTION_INTERVAL( true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_DATE				(false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_DEV_NAME			(false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_ENABLE				(false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_ID					(false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_IP					(false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_KEEP_ALIVE_INTERVAL(false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_LOCATION			(false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_LOOP_INTERVAL		(false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_NAME				(false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_PARENT_ID			(false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_REGISTERED			(false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_SCALE				(false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_SENSOR_ID			(false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_SNMP_MODULE		(false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false, false, false);
Fields	PROPERTY_SNMP_COMMUNITY		(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false, false, false, false, false, false, false);
Fields	PROPERTY_TIME				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false, false);
Fields	PROPERTY_TIMEOUT			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false, false);
Fields	PROPERTY_TYPE				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false, false);
Fields	PROPERTY_VALUE_MIN			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false, false);
Fields	PROPERTY_VALUE_MAX			(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false, false);
Fields	PROPERTY_UNIT				(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false, false);
Fields	PROPERTY_UPDATE_INTERVAL	(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true, false);
Fields	PROPERTY_TIME_OF_EXPIRE		(false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,  true);
Fields	PROPERTY_ALL				( true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Fields	PROPERTY_CREATE				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Fields	PROPERTY_ADD_CONFIRM		( true, false,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Fields	PROPERTY_GET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);
Fields	PROPERTY_SET				( true,  true,  true,  true,  true,  true,  true,  true, false,  true,  true,  true,  true,  true,  true,  true, true,  true,  true,  true,   true,  true,  true, false);

Fields::Fields()
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

Fields::Fields( bool	_correction_interval, bool	_date, bool	_dev_name, bool	_enable, bool	_id, bool	_ip, bool	_keep_alive_interval, bool	_location, bool	_loop_interval, bool	_name,
				bool	_parent_id, bool	_registered, bool	_scale, bool	_sensor_id, bool	_snmp_module, bool	_snmp_community, bool	_time, bool	_timeout, bool	_type, bool	_value_min,
				bool	_value_max, bool	_unit, bool	_update_interval, bool _time_of_expire)
:correction_interval(_correction_interval), date(date), dev_name(_dev_name), enable(_enable), id(_id), ip(_ip), keep_alive_interval(_keep_alive_interval), location(_location), loop_interval(_loop_interval), name(_name),
				parent_id(_parent_id), registered(_registered), scale(_scale), sensor_id(_sensor_id), snmp_module(_snmp_module), snmp_community(_snmp_community), time(_time), timeout(_timeout), type(_type),
				value_min(_value_min), value_max(_value_max), unit(_unit), update_interval(_update_interval), time_of_expire(_time_of_expire)
{
}

Fields	operator+(Fields& _field1, Fields& _fields2)
{
	Fields	fields;

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

Fields&	Fields::operator+=(Fields& _fields)
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



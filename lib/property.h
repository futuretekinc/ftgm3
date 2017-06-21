#ifndef	PROPERTY_H_
#define	PROPERTY_H_

#include <utility>
#include <map>
#include <list>
#include <iostream>
#include <libjson/libjson.h>
#include "value.h"
#include "time2.h"




	struct	Fields
	{
		Fields();
		Fields( bool	_correction_interval,
				bool	_date,
				bool	_port,
				bool	_enable,
				bool	_id,
				bool	_ip,
				bool	_keep_alive_interval,
				bool	_location,
				bool	_loop_interval,
				bool	_name,
				bool	_parent_id,
				bool	_registered,
				bool	_scale,
				bool	_sensor_id,
				bool	_snmp_module,
				bool	_snmp_community,
				bool	_time,
				bool	_timeout,
				bool	_type,
				bool	_value_min,
				bool	_value_max,
				bool	_unit,
				bool	_update_interval,
				bool	_time_of_expire);
		Fields&	operator+=(Fields& _fields);
		
		bool	Set(std::string const& _field);

		bool	correction_interval:1;
		bool	date:1;
		bool	port:1;
		bool	enable:1;
		bool	id:1;
		bool	ip:1;
		bool	keep_alive_interval:1;
		bool	location:1;
		bool	loop_interval:1;
		bool	name:1;
		bool	parent_id:1;
		bool	registered:1;
		bool	scale:1;
		bool	sensor_id:1;
		bool	snmp_module:1;
		bool	snmp_community:1;
		bool	time:1;
		bool	timeout:1;
		bool	type:1;
		bool	value_min:1;
		bool	value_max:1;
		bool	unit:1;
		bool	update_interval:1;
		bool	time_of_expire:1;
	};

Fields	operator+(Fields& _fields1, Fields& _fields2);

extern Fields	PROPERTY_CORRECTION_INTERVAL;
extern Fields	PROPERTY_DATE;
extern Fields	PROPERTY_DEV_NAME;
extern Fields	PROPERTY_ENABLE;	
extern Fields	PROPERTY_ID;
extern Fields	PROPERTY_IP;
extern Fields	PROPERTY_KEEP_ALIVE_INTERVAL;
extern Fields	PROPERTY_LOCATION;
extern Fields	PROPERTY_LOOP_INTERVAL;
extern Fields	PROPERTY_NAME;
extern Fields	PROPERTY_PARENT_ID;	
extern Fields	PROPERTY_REGISTERED;
extern Fields	PROPERTY_SCALE;
extern Fields	PROPERTY_SENSOR_ID;
extern Fields	PROPERTY_SNMP_MODULE;
extern Fields	PROPERTY_SNMP_COMMUNITY;
extern Fields	PROPERTY_TIME;
extern Fields	PROPERTY_TIMEOUT;
extern Fields	PROPERTY_TYPE;
extern Fields	PROPERTY_VALUE_MIN;
extern Fields	PROPERTY_VALUE_MAX;
extern Fields	PROPERTY_UNIT;
extern Fields	PROPERTY_UPDATE_INTERVAL;
extern Fields	PROPERTY_TIME_OF_EXPIRE;

extern Fields	PROPERTY_CLEAR;
extern Fields	PROPERTY_ALL;

extern Fields	PROPERTY_CREATE;
extern Fields	PROPERTY_ADD_CONFIRM;
extern Fields	PROPERTY_SET;
extern Fields	PROPERTY_GET;

#endif

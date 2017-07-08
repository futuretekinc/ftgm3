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
	Fields(uint32_t _flags);

	bool	Set(std::string const& _field);
	bool	Set(uint32_t _flags);
	bool	Reset(std::string const& _field);
	bool	Reset(uint32_t _flags);
	bool	Names(std::multimap<uint32_t, std::string>& _names);

	Fields&	operator+=(Fields& _fields);
	operator uint32_t() const;

	bool	correction_interval:1;
	bool	date:1;
	bool	options:1;
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
	bool	value:1;
	bool	stat:1;
	bool	model:1;
};

Fields	operator+(Fields& _fields1, Fields& _fields2);

#define	PROPERTY_ID_FLAG					(((uint32_t)1) <<  0)
#define	PROPERTY_TYPE_FLAG					(((uint32_t)1) <<  1)
#define	PROPERTY_MODEL_FLAG					(((uint32_t)1) <<  2)
#define	PROPERTY_NAME_FLAG					(((uint32_t)1) <<  3)
#define	PROPERTY_TIME_FLAG					(((uint32_t)1) <<  4)
#define	PROPERTY_ENABLE_FLAG				(((uint32_t)1) <<  5)	
#define	PROPERTY_STAT_FLAG					(((uint32_t)1) <<  6)
#define	PROPERTY_LOCATION_FLAG				(((uint32_t)1) <<  7)
#define	PROPERTY_REGISTERED_FLAG			(((uint32_t)1) <<  8)
#define	PROPERTY_PARENT_ID_FLAG				(((uint32_t)1) <<  9)	

#define	PROPERTY_IP_FLAG					(((uint32_t)1) <<  10)
#define	PROPERTY_SENSOR_ID_FLAG				(((uint32_t)1) <<  11)
#define	PROPERTY_UNIT_FLAG					(((uint32_t)1) <<  12)
#define	PROPERTY_SCALE_FLAG					(((uint32_t)1) <<  13)
#define	PROPERTY_VALUE_MIN_FLAG				(((uint32_t)1) <<  14)
#define	PROPERTY_VALUE_MAX_FLAG				(((uint32_t)1) <<  15)

#define	PROPERTY_OPTIONS_FLAG				(((uint32_t)1) <<  16)

#define	PROPERTY_SNMP_MODULE_FLAG			(((uint32_t)1) <<  17)
#define	PROPERTY_SNMP_COMMUNITY_FLAG		(((uint32_t)1) <<  18)
#define	PROPERTY_TIMEOUT_FLAG				(((uint32_t)1) <<  19)

#define	PROPERTY_CORRECTION_INTERVAL_FLAG	(((uint32_t)1) <<  20)
#define	PROPERTY_KEEP_ALIVE_INTERVAL_FLAG	(((uint32_t)1) <<  21)
#define	PROPERTY_LOOP_INTERVAL_FLAG			(((uint32_t)1) <<  22)
#define	PROPERTY_UPDATE_INTERVAL_FLAG		(((uint32_t)1) <<  23)
#define	PROPERTY_TIME_OF_EXPIRE_FLAG		(((uint32_t)1) <<  24)
#define	PROPERTY_VALUE_FLAG					(((uint32_t)1) <<  25)

#define	PROPERTY_STATIC_FLAG (PROPERTY_ID_FLAG	|\
							PROPERTY_TYPE_FLAG	|\
							PROPERTY_MODEL_FLAG	|\
							PROPERTY_NAME_FLAG	|\
							PROPERTY_TIME_FLAG	|\
							PROPERTY_ENABLE_FLAG|\
							PROPERTY_STAT_FLAG	|\
							PROPERTY_LOCATION_FLAG|\
							PROPERTY_REGISTERED_FLAG|\
							PROPERTY_PARENT_ID_FLAG	|\
							PROPERTY_IP_FLAG	|\
							PROPERTY_SENSOR_ID_FLAG|\
							PROPERTY_UNIT_FLAG	|\
							PROPERTY_SCALE_FLAG	|\
							PROPERTY_VALUE_MIN_FLAG|\
							PROPERTY_VALUE_MAX_FLAG|\
							PROPERTY_OPTIONS_FLAG|\
							PROPERTY_SNMP_MODULE_FLAG|\
							PROPERTY_SNMP_COMMUNITY_FLAG|\
							PROPERTY_TIMEOUT_FLAG|\
							PROPERTY_CORRECTION_INTERVAL_FLAG|\
							PROPERTY_KEEP_ALIVE_INTERVAL_FLAG)

extern Fields	PROPERTY_CORRECTION_INTERVAL;
extern Fields	PROPERTY_DATE;
extern Fields	PROPERTY_OPTIONS;
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
extern Fields	PROPERTY_VALUE;
extern Fields	PROPERTY_STATE;
extern Fields	PROPERTY_MODEL;

extern Fields	PROPERTY_CLEAR;
//extern const Fields	PROPERTY_ALL;

extern Fields	PROPERTY_CREATE;
extern Fields	PROPERTY_ADD_CONFIRM;
extern Fields	PROPERTY_SET;
extern Fields	PROPERTY_GET;

#endif

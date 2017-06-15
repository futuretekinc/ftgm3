#ifndef	PROPERTY_H_
#define	PROPERTY_H_

#include <utility>
#include <map>
#include <list>
#include <iostream>
#include <libjson/libjson.h>
#include "value.h"
#include "time2.h"




class	Properties;
class	PropertiesList;

class	Property : public std::pair<std::string,Value*>
{
public:
	Property(Property const& _property);
	Property(std::string const& _name, int _value);
	Property(std::string const& _name, uint32_t _value);
	Property(std::string const& _name, uint64_t _value);
	Property(std::string const& _name, bool _value);
	Property(std::string const& _name, float _value);
	Property(std::string const& _name, double _value);
	Property(std::string const& _name, char const* _value);
	Property(std::string const& _name, std::string const& _value);
	Property(std::string const& _name, Date const& _value);
	Property(std::string const& _name, Time const& _value);
	Property(std::string const& _name, Properties const& _vlaue);
	Property(std::string const& _name, time_t _value);
	Property(std::string const& _name, PropertiesList const& _vlaue);
	Property(std::string const& _name, ValueFloat const& _vlaue);
	~Property();

	const Property& operator=(Property const& _property);
			bool	operator=(std::string const& _vlaue);
			bool	Set(std::string const& _vlaue);
					operator JSONNode() const;

	const std::string&	GetName() const;
	const Value*		GetValue() const;
	//operator	JSONNode();
	friend	std::ostream&	::operator<<(std::ostream& os, Property const& _property);
};

class	Properties : public std::list<Property>
{
public:
	struct	Fields
	{
		Fields();
		Fields( bool	_correction_interval,
				bool	_date,
				bool	_dev_name,
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
		bool	dev_name:1;
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

	Properties();
	Properties(Properties const& _properties);
	~Properties();

	bool	Append(Property const& _property);
	bool	Append(JSONNode const& _json);

	bool	AppendCorrectionInterval(uint32_t _interval);
	bool	AppendCorrectionInterval(std::string const& _interval);

	bool	AppendDate(Date const& _date);
	bool	AppendDate(std::string const& _date);

	bool	AppendDevName(std::string const& _dev_name);

	bool	AppendEnable(ValueBool const& _enable);
	bool	AppendEnable(bool _enable);
	bool	AppendEnable(std::string	const& _enable);

	bool	AppendID(ValueID const& _id);

	bool	AppendIP(ValueIP const& _ip);
	bool	AppendIP(std::string const& _ip);

	bool	AppendKeepAliveInterval(uint32_t _interval);
	bool	AppendKeepAliveInterval(std::string const& _interval);

	bool	AppendLocation(std::string const& _location);
	bool	AppendLoopInterval(uint32_t _interval);
	bool	AppendLoopInterval(std::string const& _interval);

	bool	AppendName(ValueName const& _name);

	bool	AppendParentID(ValueID const& _device_id);
	bool	AppendParentID(std::string const& _device_id);

	bool	AppendRegistered(bool _registered);
	bool	AppendRegistered(std::string const& _registered);

	bool	AppendScale(double _scale);

	bool	AppendSensorID(std::string const& _sensor_id);

	bool	AppendSNMPModule(std::string const& _module);
	bool	AppendSNMPCommunity(std::string const& _community);

	bool	AppendTime(Date const& _date);
	bool	AppendTime(std::string const& _date);

	bool	AppendTimeOfExpire(Date const& _time_of_exire);

	bool	AppendTimeout(uint32_t	_timeout);
	bool	AppendTimeout(std::string const& _timeout);

	bool	AppendType(std::string const& _type);

	bool	AppendValueMin(ValueFloat const& _value);
	bool	AppendValueMax(ValueFloat const& _value);

	bool	AppendUnit(std::string const& _unit);
	bool	AppendUpdateInterval(uint32_t _interval);
	bool	AppendUpdateInterval(std::string const& _interval);

	bool	Delete(std::string const& _name);

	void		Clear();
	uint32_t	Count();

	const Property*	Get(std::string const& _name) const;
	
	void	ToJSON(JSONNode& _json) const;
	friend	std::ostream&	::operator<<(std::ostream& os, Properties const& _properties);

};

Properties::Fields	operator+(Properties::Fields& _fields1, Properties::Fields& _fields2);

extern Properties::Fields	PROPERTY_CORRECTION_INTERVAL;
extern Properties::Fields	PROPERTY_DATE;
extern Properties::Fields	PROPERTY_DEV_NAME;
extern Properties::Fields	PROPERTY_ENABLE;	
extern Properties::Fields	PROPERTY_ID;
extern Properties::Fields	PROPERTY_IP;
extern Properties::Fields	PROPERTY_KEEP_ALIVE_INTERVAL;
extern Properties::Fields	PROPERTY_LOCATION;
extern Properties::Fields	PROPERTY_LOOP_INTERVAL;
extern Properties::Fields	PROPERTY_NAME;
extern Properties::Fields	PROPERTY_PARENT_ID;	
extern Properties::Fields	PROPERTY_REGISTERED;
extern Properties::Fields	PROPERTY_SCALE;
extern Properties::Fields	PROPERTY_SENSOR_ID;
extern Properties::Fields	PROPERTY_SNMP_MODULE;
extern Properties::Fields	PROPERTY_SNMP_COMMUNITY;
extern Properties::Fields	PROPERTY_TIME;
extern Properties::Fields	PROPERTY_TIMEOUT;
extern Properties::Fields	PROPERTY_TYPE;
extern Properties::Fields	PROPERTY_VALUE_MIN;
extern Properties::Fields	PROPERTY_VALUE_MAX;
extern Properties::Fields	PROPERTY_UNIT;
extern Properties::Fields	PROPERTY_UPDATE_INTERVAL;
extern Properties::Fields	PROPERTY_TIME_OF_EXPIRE;

extern Properties::Fields	PROPERTY_CLEAR;
extern Properties::Fields	PROPERTY_ALL;

extern Properties::Fields	PROPERTY_CREATE;
extern Properties::Fields	PROPERTY_ADD_CONFIRM;
extern Properties::Fields	PROPERTY_SET;
extern Properties::Fields	PROPERTY_GET;

class	PropertiesList : public std::list<Properties>
{
public:
	PropertiesList();
	PropertiesList(PropertiesList const& _properties_list);
	PropertiesList(JSONNode const& _json);
	~PropertiesList();

	bool	Append(JSONNode const& _json);
	bool	Append(Properties const& _vlaue);

			operator JSONNode() const;	

	friend	std::ostream&	::operator<<(std::ostream& os, PropertiesList const& _properties_list);
};

#endif

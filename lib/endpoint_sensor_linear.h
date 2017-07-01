#ifndef	ENDPOINT_SENSOR_LINEAR_H_
#define	ENDPOINT_SENSOR_LINEAR_H_

#include "endpoint_sensor.h"

class	EndpointSensorLinear : public EndpointSensor
{
public:
	EndpointSensorLinear(ObjectManager& _manager, std::string const& _type, std::string const& _unit, double _min, double _max);
	EndpointSensorLinear(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties, std::string const& _unit, double _min, double _max);

			const char*	GetClassName();

			bool		IsValid(std::string const& _value);

			std::string	GetValue();
			double		GetFloatValue()	{	return	value_;	}
			bool		SetValue(std::string const& _value, bool _check = false);

			std::string	GetValueMin();
			double		GetFloatValueMin()	{	return	value_min_;	}
			bool		SetValueMin(std::string const& _min, bool _check = false);

			std::string	GetValueMax();
			double		GetFloatValueMax()	{	return	value_max_;	}
			bool		SetValueMax(std::string const& _max, bool _check = false);

			bool		GetProperties(JSONNode& _properties, Fields const& _fields);
			bool		SetProperty(JSONNode const& _property, bool _check = false);

	static	const char*	Type();

protected:
	virtual	bool		Add(time_t time, std::string const& _value);

			double		value_;
			double		value_min_;
			double		value_max_;
};

#endif

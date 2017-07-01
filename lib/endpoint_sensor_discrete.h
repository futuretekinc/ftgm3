#ifndef	ENDPOINT_SENSOR_DISCRETE_H_
#define	ENDPOINT_SENSOR_DISCRETE_H_

#include "endpoint_sensor.h"

class	EndpointSensorDiscrete : public EndpointSensor
{
public:
	EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type);
	EndpointSensorDiscrete(ObjectManager& _manager, std::string const& _type, JSONNode const& _properties);

			const char*	GetClassName();

			bool		IsValid(std::string const& _value);

			std::string	GetValue();
			bool		SetValue(std::string const& _value, bool _check = false);

			std::string	GetValueMin();
			bool		SetValueMin(std::string const& _min, bool _check = false);

			std::string	GetValueMax();
			bool		SetValueMax(std::string const& _max, bool _check = false);

			bool		GetProperties(JSONNode& _properties, Fields const& _fields);
			bool		SetProperty(JSONNode const& _property, bool _check = false);

	static const char*	Type();

protected:
			bool		value_;
};

#endif

#ifndef	ENDPOINT_SENSOR_TEMPERATURE_H_
#define	ENDPOINT_SENSOR_TEMPERATURE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorTemperature : public EndpointSensorLinear
{
public:
	EndpointSensorTemperature(ObjectManager& _manager);
	EndpointSensorTemperature(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

// Static member functions
public:
	static	const char*	Type();
};

#endif

#ifndef	ENDPOINT_SENSOR_TEMPERATURE_H_
#define	ENDPOINT_SENSOR_TEMPERATURE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorTemperature : public EndpointSensorLinear
{
public:
	EndpointSensorTemperature(ObjectManager& _manager);
	EndpointSensorTemperature(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();
};

#endif

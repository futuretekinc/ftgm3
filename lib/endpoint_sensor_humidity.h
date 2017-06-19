#ifndef	ENDPOINT_SENSOR_HUMIDITY_H_
#define	ENDPOINT_SENSOR_HUMIDITY_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorHumidity : public EndpointSensorLinear
{
public:
	EndpointSensorHumidity(ObjectManager& _manager);
	EndpointSensorHumidity(ObjectManager& _manager, JSONNode const& _properties);

			std::string	GetClassName();

	const 	std::string 	Type();
};

#endif

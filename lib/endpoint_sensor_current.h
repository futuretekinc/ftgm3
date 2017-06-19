#ifndef	ENDPOINT_SENSOR_CURRENT_H_
#define	ENDPOINT_SENSOR_CURRENT_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorCurrent : public EndpointSensorLinear
{
public:
	EndpointSensorCurrent(ObjectManager& _manager);
	EndpointSensorCurrent(ObjectManager& _manager, JSONNode const& _properties);

			std::string	GetClassName();

	const 	std::string 	Type();
};

#endif

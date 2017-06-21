#ifndef	ENDPOINT_SENSOR_GAS_H_
#define	ENDPOINT_SENSOR_GAS_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorGAS : public EndpointSensorLinear
{
public:
	EndpointSensorGAS(ObjectManager& _manager);
	EndpointSensorGAS(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();
};

#endif

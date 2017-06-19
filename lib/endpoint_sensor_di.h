#ifndef	ENDPOINT_SENSOR_DI_H_
#define	ENDPOINT_SENSOR_DI_H_

#include "endpoint_sensor_discrete.h"

class	EndpointSensorDI : public EndpointSensorDiscrete
{
public:
	EndpointSensorDI(ObjectManager& _manager);
	EndpointSensorDI(ObjectManager& _manager, JSONNode const& _properties);

			std::string	GetClassName();

	const 	std::string 	Type();
};

#endif

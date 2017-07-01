#ifndef	ENDPOINT_SENSOR_GAS_H_
#define	ENDPOINT_SENSOR_GAS_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorGAS : public EndpointSensorLinear
{
public:
	EndpointSensorGAS(ObjectManager& _manager);
	EndpointSensorGAS(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

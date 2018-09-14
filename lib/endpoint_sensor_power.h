#ifndef	ENDPOINT_SENSOR_POWER_H_
#define	ENDPOINT_SENSOR_POWER_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorPower : public EndpointSensorLinear
{
public:
	EndpointSensorPower(ObjectManager& _manager);
	EndpointSensorPower(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

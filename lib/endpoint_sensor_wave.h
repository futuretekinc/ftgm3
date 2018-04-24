#ifndef	ENDPOINT_SENSOR_WAVE_H_
#define	ENDPOINT_SENSOR_WAVE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorWave : public EndpointSensorLinear
{
public:
	EndpointSensorWave(ObjectManager& _manager);
	EndpointSensorWave(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

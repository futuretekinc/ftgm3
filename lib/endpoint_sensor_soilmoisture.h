#ifndef	ENDPOINT_SENSOR_SOIL_MOISTURE_H_
#define	ENDPOINT_SENSOR_SOIL_MOISTURE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorSoilMoisture : public EndpointSensorLinear
{
public:
	EndpointSensorSoilMoisture(ObjectManager& _manager);
	EndpointSensorSoilMoisture(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

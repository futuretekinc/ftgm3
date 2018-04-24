#ifndef	ENDPOINT_SENSOR_MB_DATA_H_
#define	ENDPOINT_SENSOR_MB_DATA_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorMBData : public EndpointSensorLinear
{
public:
	EndpointSensorMBData(ObjectManager& _manager);
	EndpointSensorMBData(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

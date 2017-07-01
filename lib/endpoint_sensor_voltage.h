#ifndef	ENDPOINT_SENSOR_VOLTAGE_H_
#define	ENDPOINT_SENSOR_VOLTAGE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorVoltage : public EndpointSensorLinear
{
public:
	EndpointSensorVoltage(ObjectManager& _manager);
	EndpointSensorVoltage(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

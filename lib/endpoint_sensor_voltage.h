#ifndef	ENDPOINT_SENSOR_VOLTAGE_H_
#define	ENDPOINT_SENSOR_VOLTAGE_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorVoltage : public EndpointSensorLinear
{
public:
	EndpointSensorVoltage(ObjectManager& _manager);
	EndpointSensorVoltage(ObjectManager& _manager, JSONNode const& _properties);

			std::string	GetClassName();

	const 	std::string 	Type();
};

#endif

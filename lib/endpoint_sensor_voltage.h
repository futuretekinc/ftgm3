#ifndef	ENDPOINT_SENSOR_VOLTAGE_H_
#define	ENDPOINT_SENSOR_VOLTAGE_H_

#include "endpoint_sensor.h"

class	EndpointSensorVoltage : public EndpointSensor
{
public:
	EndpointSensorVoltage(ObjectManager& _manager);
	EndpointSensorVoltage(ObjectManager& _manager, Properties const& _properties);

			std::string	GetClassName();
			bool		SetProperty(Property const& _property);

	const 	ValueType 	Type();
};

#endif

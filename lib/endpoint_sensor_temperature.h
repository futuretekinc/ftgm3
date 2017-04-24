#ifndef	ENDPOINT_SENSOR_TEMPERATURE_H_
#define	ENDPOINT_SENSOR_TEMPERATURE_H_

#include "endpoint_sensor.h"
#include "property.h"

class	EndpointSensorTemperature : public EndpointSensor
{
public:
	EndpointSensorTemperature();
	EndpointSensorTemperature(Properties const& _properties);

			std::string		GetClassName();
			Type			GetType()	const 		{	return	S_TEMPERATURE;	}
};

#endif

#ifndef	ENDPOINT_SENSOR_HUMIDITY_H_
#define	ENDPOINT_SENSOR_HUMIDITY_H_

#include "endpoint_sensor.h"

class	EndpointSensorHumidity : public EndpointSensor
{
public:
	EndpointSensorHumidity(ObjectManager& _manager);
	EndpointSensorHumidity(ObjectManager& _manager, Properties const& _properties);

			std::string	GetClassName();
			bool		SetProperty(Property const& _property);

	const 	ValueType 	Type();
};

#endif

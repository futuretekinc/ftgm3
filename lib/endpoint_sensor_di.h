#ifndef	ENDPOINT_SENSOR_DI_H_
#define	ENDPOINT_SENSOR_DI_H_

#include "endpoint_sensor.h"

class	EndpointSensorDI : public EndpointSensor
{
public:
	EndpointSensorDI(ObjectManager& _manager);
	EndpointSensorDI(ObjectManager& _manager, Properties const& _properties);

			std::string	GetClassName();
			bool		SetProperty(Property const& _property);

	const 	ValueType 	Type();
};

#endif

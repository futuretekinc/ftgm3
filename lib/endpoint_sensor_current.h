#ifndef	ENDPOINT_SENSOR_CURRENT_H_
#define	ENDPOINT_SENSOR_CURRENT_H_

#include "endpoint_sensor.h"

class	EndpointSensorCurrent : public EndpointSensor
{
public:
	EndpointSensorCurrent(ObjectManager& _manager);
	EndpointSensorCurrent(ObjectManager& _manager, Properties const& _properties);

			std::string	GetClassName();
			bool		SetProperty(Property const& _property);

	const 	ValueType 	Type();
};

#endif

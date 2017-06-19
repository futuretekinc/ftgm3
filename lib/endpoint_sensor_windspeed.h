#ifndef	ENDPOINT_SENSOR_WIND_SPEED_H_
#define	ENDPOINT_SENSOR_WIND_SPEED_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorWindSpeed : public EndpointSensorLinear
{
public:
	EndpointSensorWindSpeed(ObjectManager& _manager);
	EndpointSensorWindSpeed(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();


protected:

};

#endif

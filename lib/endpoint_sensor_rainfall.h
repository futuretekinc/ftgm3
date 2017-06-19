#ifndef	ENDPOINT_SENSOR_RAIN_FALL_H_
#define	ENDPOINT_SENSOR_RAIN_FALL_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorRainfall : public EndpointSensorLinear
{
public:
	EndpointSensorRainfall(ObjectManager& _manager);
	EndpointSensorRainfall(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();


protected:

};

#endif

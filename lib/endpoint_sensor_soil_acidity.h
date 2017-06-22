#ifndef	ENDPOINT_SENSOR_SOIL_ACIDITY_H_
#define	ENDPOINT_SENSOR_SOIL_ACIDITY_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorSoilAcidity : public EndpointSensorLinear
{
public:
	EndpointSensorSoilAcidity(ObjectManager& _manager);
	EndpointSensorSoilAcidity(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();

};

#endif

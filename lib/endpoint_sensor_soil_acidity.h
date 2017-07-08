#ifndef	ENDPOINT_SENSOR_SOIL_ACIDITY_H_
#define	ENDPOINT_SENSOR_SOIL_ACIDITY_H_

#include "endpoint_sensor_linear.h"

class	EndpointSensorSoilAcidity : public EndpointSensorLinear
{
public:
	EndpointSensorSoilAcidity(ObjectManager& _manager);
	EndpointSensorSoilAcidity(ObjectManager& _manager, JSONNode const& _properties);

			const char*	GetClassName();

		void	CorrectionProcess();
protected:

//	Static members
public:
	static	const char*	Type();
};

#endif

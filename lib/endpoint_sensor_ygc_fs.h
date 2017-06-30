#ifndef	ENDPOINT_SENSOR_YGC_FS_H_
#define	ENDPOINT_SENSOR_YGC_FS_H_

#include "endpoint_sensor_linear.h"

#define	ENDPOINT_SENSOR_YGC_FS_MIN	0
#define	ENDPOINT_SENSOR_YGC_FS_MAX	70

class	EndpointSensorYGCFS : public EndpointSensorLinear
{
public:
	EndpointSensorYGCFS(ObjectManager& _manager);
	EndpointSensorYGCFS(ObjectManager& _manager, JSONNode const& _properties);

			std::string		GetClassName();
	const 	std::string 	Type();
	const 	std::string&	GetModel() const;


protected:
	
	void	CorrectionProcess();
};

#endif

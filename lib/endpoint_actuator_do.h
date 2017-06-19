#ifndef	ENDPOINT_ACTUATOR_DO_H_
#define	ENDPOINT_ACTUATOR_DO_H_

#include "endpoint_actuator_discrete.h"

class	EndpointActuatorDO : public EndpointActuatorDiscrete
{
public:
	EndpointActuatorDO(ObjectManager& _manager);
	EndpointActuatorDO(ObjectManager& _manager, JSONNode const& _properties);

			std::string	GetClassName();

	const 	std::string 	Type();

};

#endif

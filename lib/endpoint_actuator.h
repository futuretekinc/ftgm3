#ifndef	ENDPOINT_ACTUATOR_H_
#define	ENDPOINT_ACTUATOR_H_

#include "endpoint.h"

class	EndpointActuator : public Endpoint
{
	friend	class	Device;
	friend	class	ObjectManager;

public:
	EndpointActuator(ObjectManager& _manager, std::string const& _type);

	static	bool	IsIncludeIn(Object *_object);

};

#endif

#ifndef	ENDPOINT_ACTUATOR_H_
#define	ENDPOINT_ACTUATOR_H_

#include "endpoint.h"

class	EndpointActuator : public Endpoint
{
public:
	EndpointActuator(ObjectManager& _manager, ValueType const& _type);

	bool	Set(std::string const& _value);
};

#endif

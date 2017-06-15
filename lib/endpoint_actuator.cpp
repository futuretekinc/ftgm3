#include "endpoint_actuator.h"

EndpointActuator::EndpointActuator(ObjectManager& _manager, ValueType const& _type)
:Endpoint(_manager, _type)
{
}

bool	EndpointActuator::Set(std::string const& _value)
{
	return	true;
}

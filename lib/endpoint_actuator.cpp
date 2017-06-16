#include "endpoint_actuator.h"

EndpointActuator::EndpointActuator(ObjectManager& _manager, ValueType const& _type)
:Endpoint(_manager, _type)
{
}

bool	EndpointActuator::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<EndpointActuator*>(_object) != NULL;
}

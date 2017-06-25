#include "endpoint_actuator.h"

EndpointActuator::EndpointActuator(ObjectManager& _manager, std::string const& _type)
:Endpoint(_manager, _type)
{
	JSONNode	test;
	GetProperties(test);
}

bool	EndpointActuator::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<EndpointActuator*>(_object) != NULL;
}

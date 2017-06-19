#include "defined.h"
#include "json.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint_actuator_do.h"

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager)
: EndpointActuatorDiscrete(_manager, EndpointActuatorDO::Type())
{
	trace.SetClassName(GetClassName());
}

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager, JSONNode const& _properties)
: EndpointActuatorDiscrete(_manager, EndpointActuatorDO::Type())
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointActuatorDO::GetClassName()
{
	return	"EPSDO";
}

const std::string EndpointActuatorDO::Type()
{
	return	std::string(NODE_TYPE_EP_A_DO);
}



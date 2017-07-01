#include "defined.h"
#include "json.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint_actuator_do.h"

static const char* class_name = "EPSDO";

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager)
: EndpointActuatorDiscrete(_manager, EndpointActuatorDO::Type())
{
	trace.SetClassName(class_name);
}

EndpointActuatorDO::EndpointActuatorDO(ObjectManager& _manager, JSONNode const& _properties)
: EndpointActuatorDiscrete(_manager, EndpointActuatorDO::Type())
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char* EndpointActuatorDO::GetClassName()
{
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointActuatorDO::Type()
{
	return	OBJECT_TYPE_EP_A_DO;
}



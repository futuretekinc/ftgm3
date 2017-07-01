#include "defined.h"
#include "device.h"
#include "endpoint_sensor_di.h"

static const char* class_name = "EP_S_DI";

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager)
: EndpointSensorDiscrete(_manager, EndpointSensorDI::Type())
{
	trace.SetClassName(class_name);
}

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorDiscrete(_manager, EndpointSensorDI::Type())
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char* EndpointSensorDI::GetClassName()
{
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorDI::Type()
{
	return	OBJECT_TYPE_EP_S_DI;
}

#include "defined.h"
#include "device.h"
#include "endpoint_sensor_di.h"

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager)
: EndpointSensorDiscrete(_manager, EndpointSensorDI::Type())
{
	trace.SetClassName(GetClassName());
}

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorDiscrete(_manager, EndpointSensorDI::Type())
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointSensorDI::GetClassName()
{
	return	"EP_S_DI";
}

const std::string EndpointSensorDI::Type()
{
	return	std::string(NODE_TYPE_EP_S_DI);
}

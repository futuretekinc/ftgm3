#include "defined.h"
#include "device.h"
#include "endpoint_sensor_current.h"

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointSensorCurrent::GetClassName()
{
	return	"EPSCurrent";
}

const std::string EndpointSensorCurrent::Type()
{
	return	std::string(NODE_TYPE_EP_S_CURRENT);
}

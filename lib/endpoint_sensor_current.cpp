#include "defined.h"
#include "device.h"
#include "endpoint_sensor_current.h"

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager)
: EndpointSensor(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , PROPERTY_ALL, true);
}

std::string	EndpointSensorCurrent::GetClassName()
{
	return	"EPSCurrent";
}

const ValueType EndpointSensorCurrent::Type()
{
	return	ValueType(NODE_TYPE_EP_S_CURRENT);
}

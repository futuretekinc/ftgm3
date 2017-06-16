#include "defined.h"
#include "device.h"
#include "endpoint_sensor_di.h"

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager)
: EndpointSensor(_manager, EndpointSensorDI::Type(), ENDPOINT_SENSOR_DI_UNIT, ENDPOINT_SENSOR_DI_MIN, ENDPOINT_SENSOR_DI_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorDI::EndpointSensorDI(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, EndpointSensorDI::Type(), ENDPOINT_SENSOR_DI_UNIT, ENDPOINT_SENSOR_DI_MIN, ENDPOINT_SENSOR_DI_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , PROPERTY_ALL, true);
}

std::string	EndpointSensorDI::GetClassName()
{
	return	"EP_S_DI";
}

const ValueType EndpointSensorDI::Type()
{
	return	ValueType(NODE_TYPE_EP_S_DI);
}

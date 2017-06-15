#include "defined.h"
#include "device.h"
#include "endpoint_sensor_humidity.h"

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager)
: EndpointSensor(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , PROPERTY_ALL, true);
}

std::string	EndpointSensorHumidity::GetClassName()
{
	return	"EPSHumidity";
}

const ValueType EndpointSensorHumidity::Type()
{
	return	ValueType(NODE_TYPE_EP_S_HUMIDITY);
}

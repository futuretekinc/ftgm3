#include "defined.h"
#include "device.h"
#include "endpoint_sensor_humidity.h"

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointSensorHumidity::GetClassName()
{
	return	"EPSHumidity";
}

const std::string EndpointSensorHumidity::Type()
{
	return	std::string (NODE_TYPE_EP_S_HUMIDITY);
}

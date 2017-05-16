#include "defined.h"
#include "device.h"
#include "endpoint_sensor_humidity.h"

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager)
: EndpointSensor(_manager, ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
}

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
	SetProperties(_properties, true);
}

std::string	EndpointSensorHumidity::GetClassName()
{
	return	"EPSHumidity";
}



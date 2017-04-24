#include "defined.h"
#include "device.h"
#include "endpoint_sensor_humidity.h"

EndpointSensorHumidity::EndpointSensorHumidity()
: EndpointSensor(ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorHumidity::EndpointSensorHumidity(Properties const& _properties)
: EndpointSensor(ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, true);
}

std::string	EndpointSensorHumidity::GetClassName()
{
	return	"EPSHumidity";
}



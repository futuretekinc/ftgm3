#include "defined.h"
#include "endpoint_sensor_temperature.h"

EndpointSensorTemperature::EndpointSensorTemperature()
: EndpointSensor(ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
}

EndpointSensorTemperature::EndpointSensorTemperature(Properties const& _properties)
: EndpointSensor(ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	SetProperties(_properties, true);
}



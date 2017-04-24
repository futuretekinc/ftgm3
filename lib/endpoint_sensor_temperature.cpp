#include "defined.h"
#include "device.h"
#include "endpoint_sensor_temperature.h"

EndpointSensorTemperature::EndpointSensorTemperature()
: EndpointSensor(ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorTemperature::EndpointSensorTemperature(Properties const& _properties)
: EndpointSensor(ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, true);
}

std::string EndpointSensorTemperature::GetClassName() 
{	
	return	"EPSTemperature";	
}



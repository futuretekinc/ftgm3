#include "defined.h"
#include "device.h"
#include "endpoint_sensor_temperature.h"

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager)
: EndpointSensor(_manager, ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
}

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
	TRACE_CREATE;
	SetProperties(_properties, true);
}

std::string EndpointSensorTemperature::GetClassName() 
{	
	return	"EPSTemperature";	
}



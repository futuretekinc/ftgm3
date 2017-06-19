#include "defined.h"
#include "device.h"
#include "endpoint_sensor_temperature.h"

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorTemperature::GetClassName() 
{	
	return	"EPSTemperature";	
}

const 	std::string 	EndpointSensorTemperature::Type()
{
	return	std::string(NODE_TYPE_EP_S_TEMPERATURE);
}


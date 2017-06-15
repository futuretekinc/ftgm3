#include "defined.h"
#include "device.h"
#include "endpoint_sensor_temperature.h"

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager)
: EndpointSensor(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, PROPERTY_ALL, true);
}

std::string EndpointSensorTemperature::GetClassName() 
{	
	return	"EPSTemperature";	
}

const 	ValueType 	EndpointSensorTemperature::Type()
{
	return	ValueType(NODE_TYPE_EP_S_TEMPERATURE);
}

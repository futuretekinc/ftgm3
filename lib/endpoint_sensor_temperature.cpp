#include "defined.h"
#include "device.h"
#include "endpoint_sensor_temperature.h"

static const char*	class_name = "EPSTemperature";	

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorTemperature::EndpointSensorTemperature(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorTemperature::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char*	EndpointSensorTemperature::GetClassName() 
{	
	return	"EPSTemperature";	
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorTemperature::Type()
{
	return	OBJECT_TYPE_EP_S_TEMPERATURE;
}


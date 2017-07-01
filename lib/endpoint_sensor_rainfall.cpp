#include "defined.h"
#include "device.h"
#include "endpoint_sensor_rainfall.h"

static const char* class_name = "EPSRainfall";	

EndpointSensorRainfall::EndpointSensorRainfall(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorRainfall::Type(), ENDPOINT_SENSOR_RAINFALL_UNIT, ENDPOINT_SENSOR_RAINFALL_MIN, ENDPOINT_SENSOR_RAINFALL_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorRainfall::EndpointSensorRainfall(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorRainfall::Type(), ENDPOINT_SENSOR_RAINFALL_UNIT, ENDPOINT_SENSOR_RAINFALL_MIN, ENDPOINT_SENSOR_RAINFALL_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char* EndpointSensorRainfall::GetClassName() 
{	
	return	class_name;	
}

const char*	EndpointSensorRainfall::Type()
{
	return	OBJECT_TYPE_EP_S_RAINFALL;
}


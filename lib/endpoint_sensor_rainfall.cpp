#include "defined.h"
#include "device.h"
#include "endpoint_sensor_rainfall.h"

EndpointSensorRainfall::EndpointSensorRainfall(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorRainfall::Type(), ENDPOINT_SENSOR_RAINFALL_UNIT, ENDPOINT_SENSOR_RAINFALL_MIN, ENDPOINT_SENSOR_RAINFALL_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorRainfall::EndpointSensorRainfall(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorRainfall::Type(), ENDPOINT_SENSOR_RAINFALL_UNIT, ENDPOINT_SENSOR_RAINFALL_MIN, ENDPOINT_SENSOR_RAINFALL_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorRainfall::GetClassName() 
{	
	return	"EPSRainfall";	
}

const 	std::string 	EndpointSensorRainfall::Type()
{
	return	std::string(NODE_TYPE_EP_S_RAINFALL);
}


#include "defined.h"
#include "device.h"
#include "endpoint_sensor_windspeed.h"

EndpointSensorWindSpeed::EndpointSensorWindSpeed(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorWindSpeed::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorWindSpeed::EndpointSensorWindSpeed(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorWindSpeed::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorWindSpeed::GetClassName() 
{	
	return	"EPSWindSpeed";	
}

const 	std::string 	EndpointSensorWindSpeed::Type()
{
	return	std::string(NODE_TYPE_EP_S_WIND_SPEED);
}


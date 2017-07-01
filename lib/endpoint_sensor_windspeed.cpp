#include "defined.h"
#include "device.h"
#include "endpoint_sensor_windspeed.h"

static const char* class_name = "EPSWindSpeed";	

EndpointSensorWindSpeed::EndpointSensorWindSpeed(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorWindSpeed::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorWindSpeed::EndpointSensorWindSpeed(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorWindSpeed::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char* EndpointSensorWindSpeed::GetClassName() 
{	
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorWindSpeed::Type()
{
	return	OBJECT_TYPE_EP_S_WIND_SPEED;
}


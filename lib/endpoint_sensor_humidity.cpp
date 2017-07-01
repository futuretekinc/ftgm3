#include "defined.h"
#include "device.h"
#include "endpoint_sensor_humidity.h"

static const char*	class_name = "EPSHumidity";

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorHumidity::EndpointSensorHumidity(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorHumidity::Type(), ENDPOINT_SENSOR_HUMIDITY_UNIT, ENDPOINT_SENSOR_HUMIDITY_MIN, ENDPOINT_SENSOR_HUMIDITY_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char*	EndpointSensorHumidity::GetClassName()
{
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorHumidity::Type()
{
	return	OBJECT_TYPE_EP_S_HUMIDITY;
}

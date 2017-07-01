#include "defined.h"
#include "device.h"
#include "endpoint_sensor_current.h"

static const char*	class_name = "EPSCurrent";

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorCurrent::EndpointSensorCurrent(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorCurrent::Type(), ENDPOINT_SENSOR_CURRENT_UNIT, ENDPOINT_SENSOR_CURRENT_MIN, ENDPOINT_SENSOR_CURRENT_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char*	EndpointSensorCurrent::GetClassName()
{
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorCurrent::Type()
{
	return	OBJECT_TYPE_EP_S_CURRENT;
}

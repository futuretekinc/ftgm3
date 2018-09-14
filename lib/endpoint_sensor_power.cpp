#include "defined.h"
#include "device.h"
#include "endpoint_sensor_power.h"

static const char*	class_name = "EPSPower";

EndpointSensorPower::EndpointSensorPower(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorPower::Type(), ENDPOINT_SENSOR_POWER_UNIT, ENDPOINT_SENSOR_POWER_MIN, ENDPOINT_SENSOR_POWER_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorPower::EndpointSensorPower(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorPower::Type(), ENDPOINT_SENSOR_POWER_UNIT, ENDPOINT_SENSOR_POWER_MIN, ENDPOINT_SENSOR_POWER_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char*	EndpointSensorPower::GetClassName()
{
	return	"EPSPower";
}

const char*	EndpointSensorPower::Type()
{
	return	OBJECT_TYPE_EP_S_POWER;
}

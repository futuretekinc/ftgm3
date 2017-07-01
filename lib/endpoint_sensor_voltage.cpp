#include "defined.h"
#include "device.h"
#include "endpoint_sensor_voltage.h"

static const char*	class_name = "EPSVoltage";

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char*	EndpointSensorVoltage::GetClassName()
{
	return	"EPSVoltage";
}

const char*	EndpointSensorVoltage::Type()
{
	return	OBJECT_TYPE_EP_S_VOLTAGE;
}

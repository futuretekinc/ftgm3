#include "defined.h"
#include "device.h"
#include "endpoint_sensor_voltage.h"

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , false, true);
}

std::string	EndpointSensorVoltage::GetClassName()
{
	return	"EPSVoltage";
}

const std::string EndpointSensorVoltage::Type()
{
	return	std::string (NODE_TYPE_EP_S_VOLTAGE);
}

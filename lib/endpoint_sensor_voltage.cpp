#include "defined.h"
#include "device.h"
#include "endpoint_sensor_voltage.h"

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager)
: EndpointSensor(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorVoltage::EndpointSensorVoltage(ObjectManager& _manager, Properties const& _properties)
: EndpointSensor(_manager, EndpointSensorVoltage::Type(), ENDPOINT_SENSOR_VOLTAGE_UNIT, ENDPOINT_SENSOR_VOLTAGE_MIN, ENDPOINT_SENSOR_VOLTAGE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties , PROPERTY_ALL, true);
}

std::string	EndpointSensorVoltage::GetClassName()
{
	return	"EPSVoltage";
}

const ValueType EndpointSensorVoltage::Type()
{
	return	ValueType(NODE_TYPE_EP_S_VOLTAGE);
}

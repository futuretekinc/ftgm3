#include "defined.h"
#include "device.h"
#include "endpoint_sensor_gas.h"

EndpointSensorGAS::EndpointSensorGAS(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorGAS::Type(), ENDPOINT_SENSOR_GAS_UNIT, ENDPOINT_SENSOR_GAS_MIN, ENDPOINT_SENSOR_GAS_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorGAS::EndpointSensorGAS(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorGAS::Type(), ENDPOINT_SENSOR_GAS_UNIT, ENDPOINT_SENSOR_GAS_MIN, ENDPOINT_SENSOR_GAS_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorGAS::GetClassName() 
{	
	return	"EPSGAS";	
}

const 	std::string 	EndpointSensorGAS::Type()
{
	return	std::string(NODE_TYPE_EP_S_GAS);
}


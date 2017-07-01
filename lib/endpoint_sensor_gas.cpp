#include "defined.h"
#include "device.h"
#include "endpoint_sensor_gas.h"

static const char* class_name = "EPSGas";	

EndpointSensorGAS::EndpointSensorGAS(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorGAS::Type(), ENDPOINT_SENSOR_GAS_UNIT, ENDPOINT_SENSOR_GAS_MIN, ENDPOINT_SENSOR_GAS_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorGAS::EndpointSensorGAS(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorGAS::Type(), ENDPOINT_SENSOR_GAS_UNIT, ENDPOINT_SENSOR_GAS_MIN, ENDPOINT_SENSOR_GAS_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char*	EndpointSensorGAS::GetClassName() 
{	
	return	class_name;	
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorGAS::Type()
{
	return	OBJECT_TYPE_EP_S_GAS;
}


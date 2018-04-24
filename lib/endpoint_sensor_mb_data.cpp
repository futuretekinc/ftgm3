#include "defined.h"
#include "device.h"
#include "endpoint_sensor_mb_data.h"

static const char* class_name = "EPSModbus_data";	

EndpointSensorMBData::EndpointSensorMBData(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorMBData::Type(), ENDPOINT_SENSOR_MB_DATA_UNIT, ENDPOINT_SENSOR_MB_DATA_MIN, ENDPOINT_SENSOR_MB_DATA_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorMBData::EndpointSensorMBData(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorMBData::Type(), ENDPOINT_SENSOR_MB_DATA_UNIT, ENDPOINT_SENSOR_MB_DATA_MIN, ENDPOINT_SENSOR_MB_DATA_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char*	EndpointSensorMBData::GetClassName() 
{	
	return	class_name;	
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorMBData::Type()
{
	return	OBJECT_TYPE_EP_S_MB_DATA;
}


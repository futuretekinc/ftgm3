#include "defined.h"
#include "device.h"
#include "endpoint_sensor_soilmoisture.h"

static const char*	class_name = "EPSSoilMoisture";	

EndpointSensorSoilMoisture::EndpointSensorSoilMoisture(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorSoilMoisture::Type(), ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT, ENDPOINT_SENSOR_SOIL_MOISTURE_MIN, ENDPOINT_SENSOR_SOIL_MOISTURE_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorSoilMoisture::EndpointSensorSoilMoisture(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorSoilMoisture::Type(), ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT, ENDPOINT_SENSOR_SOIL_MOISTURE_MIN, ENDPOINT_SENSOR_SOIL_MOISTURE_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char*	EndpointSensorSoilMoisture::GetClassName() 
{	
	return	class_name;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointSensorSoilMoisture::Type()
{
	return	OBJECT_TYPE_EP_S_SOIL_MOISTURE;
}


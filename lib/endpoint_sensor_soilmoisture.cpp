#include "defined.h"
#include "device.h"
#include "endpoint_sensor_soilmoisture.h"

EndpointSensorSoilMoisture::EndpointSensorSoilMoisture(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorSoilMoisture::Type(), ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT, ENDPOINT_SENSOR_SOIL_MOISTURE_MIN, ENDPOINT_SENSOR_SOIL_MOISTURE_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorSoilMoisture::EndpointSensorSoilMoisture(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorSoilMoisture::Type(), ENDPOINT_SENSOR_SOIL_MOISTURE_UNIT, ENDPOINT_SENSOR_SOIL_MOISTURE_MIN, ENDPOINT_SENSOR_SOIL_MOISTURE_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorSoilMoisture::GetClassName() 
{	
	return	"EPSSoilMoisture";	
}

const 	std::string 	EndpointSensorSoilMoisture::Type()
{
	return	std::string(NODE_TYPE_EP_S_SOIL_MOISTURE);
}


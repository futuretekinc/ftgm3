#include "defined.h"
#include "device.h"
#include "endpoint_sensor_soil_acidity.h"

EndpointSensorSoilAcidity::EndpointSensorSoilAcidity(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorSoilAcidity::Type(), ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT, ENDPOINT_SENSOR_SOIL_ACIDITY_MIN, ENDPOINT_SENSOR_SOIL_ACIDITY_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorSoilAcidity::EndpointSensorSoilAcidity(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorSoilAcidity::Type(), ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT, ENDPOINT_SENSOR_SOIL_ACIDITY_MIN, ENDPOINT_SENSOR_SOIL_ACIDITY_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorSoilAcidity::GetClassName() 
{	
	return	"EPSSoilAcidity";	
}

const 	std::string 	EndpointSensorSoilAcidity::Type()
{
	return	std::string(NODE_TYPE_EP_S_SOIL_ACIDITY);
}


#include "defined.h"
#include "device.h"
#include "endpoint_sensor_soil_acidity.h"

static const char* class_name = "EPSSoilAcidity";	

EndpointSensorSoilAcidity::EndpointSensorSoilAcidity(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorSoilAcidity::Type(), ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT, ENDPOINT_SENSOR_SOIL_ACIDITY_MIN, ENDPOINT_SENSOR_SOIL_ACIDITY_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorSoilAcidity::EndpointSensorSoilAcidity(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorSoilAcidity::Type(), ENDPOINT_SENSOR_SOIL_ACIDITY_UNIT, ENDPOINT_SENSOR_SOIL_ACIDITY_MIN, ENDPOINT_SENSOR_SOIL_ACIDITY_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
}

const char* EndpointSensorSoilAcidity::GetClassName() 
{	
	return	"EPSSoilAcidity";	
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorSoilAcidity::Type()
{
	return	OBJECT_TYPE_EP_S_SOIL_ACIDITY;
}


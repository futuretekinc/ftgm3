#include "defined.h"
#include "json.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint_actuator_fx3dtempctr.h"

static const char* class_name = "EPAFX3DTempCTR";

EndpointActuatorFX3DTempCTR::EndpointActuatorFX3DTempCTR(ObjectManager& _manager)
: EndpointActuatorTempControl(_manager, EndpointActuatorFX3DTempCTR::Type(),ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(class_name);
}

EndpointActuatorFX3DTempCTR::EndpointActuatorFX3DTempCTR(ObjectManager& _manager, JSONNode const& _properties)
: EndpointActuatorTempControl(_manager, EndpointActuatorFX3DTempCTR::Type(), ENDPOINT_SENSOR_TEMPERATURE_UNIT, ENDPOINT_SENSOR_TEMPERATURE_MIN, ENDPOINT_SENSOR_TEMPERATURE_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char* EndpointActuatorFX3DTempCTR::GetClassName()
{
	return	class_name;
}


////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointActuatorFX3DTempCTR::Type()
{
	return	OBJECT_TYPE_EP_A_FX3DTEMPCTR;
}



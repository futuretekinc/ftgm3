#include "defined.h"
#include "device.h"
#include "endpoint_sensor_wave.h"

static const char*	class_name = "EPSWAVE";

EndpointSensorWave::EndpointSensorWave(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorWave::Type(), ENDPOINT_SENSOR_WAVE_UNIT, ENDPOINT_SENSOR_WAVE_MIN, ENDPOINT_SENSOR_WAVE_MAX)
{
	trace.SetClassName(class_name);
}

EndpointSensorWave::EndpointSensorWave(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorWave::Type(), ENDPOINT_SENSOR_WAVE_UNIT, ENDPOINT_SENSOR_WAVE_MIN, ENDPOINT_SENSOR_WAVE_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);
}

const char*	EndpointSensorWave::GetClassName()
{
	return	"EPSWave";
}

const char*	EndpointSensorWave::Type()
{
	return	OBJECT_TYPE_EP_S_WAVE;
}

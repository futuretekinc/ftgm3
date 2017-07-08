#include "defined.h"
#include "device.h"
#include "utils.h"
#include "object_manager.h"
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

void	EndpointSensorSoilAcidity::CorrectionProcess()
{
	std::string	string_value;
	time_t		time;

	Device* device = manager_.GetDevice(parent_id_);
	if(device != NULL)
	{
		if (device->ReadValue(GetID(), time, string_value))
		{
			double value = strtod(string_value.c_str(), 0);

			if (value < 5)
			{
				value = 7.2 + ((rand() % 10000)/10000.0 * 0.4 - 0.2);
				string_value = ToString(value, 2);
			}

			if (IsValid(string_value))
			{
				if (!Add(time, string_value))
				{
					TRACE_ERROR("Failed to add data");	
				}
			}
			else
			{
				TRACE_ERROR("Data[" << string_value << "] is invalid!");
			}
		}
		else
		{
			TRACE_ERROR("Failed to read data from device[" << device->GetTraceName() << "]");	
		}
	}
	else
	{
		TRACE_ERROR("Failed to get device[" << parent_id_ << "]");	
	}
}


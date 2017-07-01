#include "defined.h"
#include "trace.h"
#include "device.h"
#include "utils.h"
#include "object_manager.h"
#include "endpoint_sensor_ygc_fs.h"

static const char* class_name = "EPSWindSpeedYGCFS";	
static const char* model_name = "YGC-FS";	

EndpointSensorYGCFS::EndpointSensorYGCFS(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorYGCFS::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(class_name);
	TRACE_INFO("The Wind Spped endpoint[" << id_ <<"] created");
}

EndpointSensorYGCFS::EndpointSensorYGCFS(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorYGCFS::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(class_name);
	SetProperties(_properties, false, true);
	TRACE_INFO("The Wind Spped endpoint[" << id_ <<"] created");
}

const char* EndpointSensorYGCFS::GetClassName() 
{	
	return	class_name;
}

std::string	EndpointSensorYGCFS::GetModel()
{
	return	std::string(EndpointSensorYGCFS::Model());
}

void	EndpointSensorYGCFS::CorrectionProcess()
{
	time_t		time;
	uint32_t	counter;

	Device* device = manager_.GetDevice(parent_id_);
	if(device != NULL)
	{
		if (device->ReadValue(GetSensorID(), time, counter))
		{
			double	wind_speed = 0;
			uint32_t	frequency = (counter % 65536) / 10;

			if (frequency > 0)
			{
				wind_speed = 0.1 + 0.0875 * frequency;
			}

			if (wind_speed < value_min_)
			{
				value_ = value_min_;
			}
			else if (wind_speed > value_max_)
			{
				value_ = value_max_;
			}
			else
			{
				value_ = wind_speed;	
			}

			TRACE_INFO("Counter : " << counter << ", Frequency : " << frequency << ", WindSpeed : " << ToString(value_, 2) << " m/s");

			if (!Add(time, ToString(value_, 2)))
			{
				TRACE_ERROR("Failed to add data");	
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

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	EndpointSensorYGCFS::Type()
{
	return	OBJECT_TYPE_EP_S_WIND_SPEED;
}

const char* EndpointSensorYGCFS::Model()
{	
	return	model_name;
}


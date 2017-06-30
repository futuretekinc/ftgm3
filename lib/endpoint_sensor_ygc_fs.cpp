#include "defined.h"
#include "device.h"
#include "utils.h"
#include "object_manager.h"
#include "endpoint_sensor_ygc_fs.h"

EndpointSensorYGCFS::EndpointSensorYGCFS(ObjectManager& _manager)
: EndpointSensorLinear(_manager, EndpointSensorYGCFS::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(GetClassName());
}

EndpointSensorYGCFS::EndpointSensorYGCFS(ObjectManager& _manager, JSONNode const& _properties)
: EndpointSensorLinear(_manager, EndpointSensorYGCFS::Type(), ENDPOINT_SENSOR_WIND_SPEED_UNIT, ENDPOINT_SENSOR_WIND_SPEED_MIN, ENDPOINT_SENSOR_WIND_SPEED_MAX)
{
	trace.SetClassName(GetClassName());
	SetProperties(_properties, false, true);
}

std::string EndpointSensorYGCFS::GetClassName() 
{	
	return	"EPSWindSpeedYGCFS";	
}

const 	std::string 	EndpointSensorYGCFS::Type()
{
	return	std::string(NODE_TYPE_EP_S_WIND_SPEED);
}

const std::string&	EndpointSensorYGCFS::GetModel() const
{	
	const static std::string model = "YGC-FS";

	return	model;
}

void	EndpointSensorYGCFS::CorrectionProcess()
{
	time_t		time;
	uint32_t	frequency;

	Device* device = manager_.GetDevice(parent_id_);
	if(device != NULL)
	{
		if (device->ReadValue(GetID(), time, frequency))
		{
			double	wind_speed = 0;

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

			TRACE_INFO("WindSpeed : " << ToString(value_, 2) << " m/s");

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

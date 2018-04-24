#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_mb7092.h"


static const char*	class_name = "DeviceHS1000M";

DeviceMB7092::DeviceMB7092(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_MB7092, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceMB7092::DeviceMB7092(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_MB7092, true), correction_interval_(1)
{
	TRACE_INFO("MB7092 Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceMB7092::GetClassName()
{
	return	class_name;
}

bool	DeviceMB7092::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	try
	{
		
		Endpoint*	endpoint = manager_.GetEndpoint(_epid);
		if (endpoint == NULL)
		{
			THROW_OBJECT_NOT_FOUND("The endpoint[" << _epid << "] is not attached");
			return	false;	
		}

		uint32_t	type 	= endpoint->GetSensorID() / 10000;
		uint32_t	index 	= endpoint->GetSensorID() % 10000 - 1; 

		if ((type != 3) || (index > 1))
		{
			TRACE_ERROR("Invalid sensro id!");
			return	false;
		}
		
		_time = time_;
		std::string _data(reinterpret_cast<char *>(data_));
		_value = _data;
		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	DeviceMB7092::Preprocess()
{
	DeviceModbus::Preprocess();
}


void	DeviceMB7092::Process()
{
	float p = 0;
	uint8_t Req_msg[6] = {0x00};
	uint8_t Resp_msg[5];
	uint32_t Resp_msg_len;
	
	if(correction_timer_.RemainTime() == 0)
	{

		TRACE_INFO("MB7092 read data");

		if (!RequestAndWait(Req_msg, sizeof(Req_msg), Resp_msg, sizeof(Resp_msg), Resp_msg_len, 2))
		{
			TRACE_ERROR("Failed to read register");
			time_ = Date::GetCurrent();
			memset( data_, 0x00, 6);
		}
		else
		{
			time_ = Date::GetCurrent();
			memcpy(data_, Resp_msg+1, 3);
			data_[3] = 0x00;
			TRACE_INFO("Success to read register :" << data_);
		}
		
		correction_timer_.Add(correction_interval_);
		TRACE_INFO("MB7092 Remain Time : " << correction_timer_.RemainTime());
	}

	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceMB7092::Type()
{
	return	OBJECT_TYPE_DEV_MB7092;
}


#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_hs_1000m.h"


static const char*	class_name = "DeviceHS1000M";

DeviceHS1000M::DeviceHS1000M(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_HS_1000M, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceHS1000M::DeviceHS1000M(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_HS_1000M, true), correction_interval_(1)
{
	TRACE_INFO("HS1000M Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceHS1000M::GetClassName()
{
	return	class_name;
}

bool	DeviceHS1000M::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	try
	{
		
		//string _data;
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
		//_value = ToString(registers_[index]);
		//_data = data_;
		//_value = _data;

		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	DeviceHS1000M::Preprocess()
{
	DeviceModbus::Preprocess();
}



uint8_t DeviceHS1000M::EOR_checksum(uint8_t* buf, uint32_t buf_len)
{
	uint8_t checksum = 0x00;
	uint32_t count;

	for(count = 0 ; count < buf_len ; count++)
	{
		checksum = checksum ^ buf[count];
	}

	return checksum;
}

void	DeviceHS1000M::Process()
{
	float p = 0;
	uint8_t Req_msg[6];
	uint8_t Resp_msg[11];
	uint32_t Resp_msg_len;
	
	if(correction_timer_.RemainTime() == 0)
	{

		TRACE_INFO("HS1000M read data");
		Req_msg[0] = 0x02;
		Req_msg[1] = 0x30;
		Req_msg[2] = 0x31;
		Req_msg[3] = 0x52;
		Req_msg[4] = 0x03;
		Req_msg[5] = EOR_checksum(Req_msg, 5);

		if (!RequestAndWait(Req_msg, sizeof(Req_msg), Resp_msg, sizeof(Resp_msg), Resp_msg_len, 2))
		{
			TRACE_ERROR("Failed to read register");
			time_ = Date::GetCurrent();
			memset( data_, 0x00, 6);
		}
		else
		{
			if(Resp_msg[Resp_msg_len-1] != EOR_checksum(Resp_msg, Resp_msg_len -2))
			{
				memset( data_, 0x00, 6 );
			}
			time_ = Date::GetCurrent();
			memcpy(data_, Resp_msg+4, 6);
			data_[5] = 0x00;
		}
		

		correction_timer_.Add(correction_interval_);
		TRACE_INFO("HS-1000M Remain Time : " << correction_timer_.RemainTime());
	}

	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceHS1000M::Type()
{
	return	OBJECT_TYPE_DEV_HS_1000M;
}


#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_ftm80_base.h"
#include "crc16.h"
#include "SLABHIDtoUART.h"
static const char*	class_name = "DeviceFTM80_BASE";
Device_ftm80_base::Device_ftm80_base(ObjectManager& _manager)
: DeviceU2U(_manager, OBJECT_TYPE_DEV_FTM80_BASE, false), correction_interval_(1)
{
	serial_u2u_.SetBaudrate(115200);
	serial_u2u_.SetParityBit("none");
	serial_u2u_.SetDataBit(8);
}

Device_ftm80_base::Device_ftm80_base(ObjectManager& _manager, JSONNode const& _properties)
: DeviceU2U(_manager, OBJECT_TYPE_DEV_FTM80_BASE, false), correction_interval_(1)
{
	TRACE_INFO("FTM80_BASE Created");

	serial_u2u_.SetBaudrate(115200);
	serial_u2u_.SetParityBit("none");
	serial_u2u_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	Device_ftm80_base::GetClassName()
{
	return	class_name;
}

bool    Device_ftm80_base::WriteValue(std::string const& _epid, bool  _value)
{
	bool    ret_value = false;
 
	Endpoint*       endpoint = manager_.GetEndpoint(_epid);
	if (endpoint != NULL)
	{
		uint32_t        type    = endpoint->GetSensorID() / 10000;
		uint32_t        index   = endpoint->GetSensorID() % 10000 - 1;
		
		if(index > 10)
		{
			TRACE_ERROR("Invalid sernsor id");
			return false;
		}
		uint8_t		request_msg[12];
		memset(request_msg, 0x00, 12);
		uint16_t	request_msg_size = 0;
		int	i_value = (int)_value;

		request_msg[request_msg_size++] = 0x01;
		request_msg[request_msg_size++] = 0x01;
		request_msg[request_msg_size++] = (FTM80_OID_DATA[index].oid_ >> 24) & 0xff;
		request_msg[request_msg_size++] = (FTM80_OID_DATA[index].oid_ >> 16) & 0xff;
		request_msg[request_msg_size++] = (FTM80_OID_DATA[index].oid_ >> 8 )& 0xff;
		request_msg[request_msg_size++] = FTM80_OID_DATA[index].oid_ & 0xff;
		request_msg[request_msg_size++] = (i_value >> 24) & 0xff;
		request_msg[request_msg_size++] = (i_value >> 16) & 0xff;
		request_msg[request_msg_size++] = (i_value >> 8) & 0xff;
		request_msg[request_msg_size++] = i_value & 0xff;
		uint16_t crc = crc16(request_msg, request_msg_size);
		request_msg[request_msg_size++] = crc & 0xff;
		request_msg[request_msg_size++] = (crc >> 8) & 0xff;

		ret_value = serial_u2u_.Write(request_msg, request_msg_size);
		TRACE_INFO("WRITE SUCCESS");
		if (ret_value)
		{
			TRACE_INFO("The endpoint[" << ToString(FTM80_OID_DATA[index].oid_) << "] : " << _value);
			FTM80_OID_DATA[index].data_ = i_value;
    		}
		else
		{
   			TRACE_INFO("Failed to write value from The endpoint[" << ToString(FTM80_OID_DATA[index].oid_) << "]");
  		}
	}
	else
	{
   		TRACE_ERROR("The endpoint[" << _epid << "] is not attached");
   	}
 
	return  ret_value;
}



bool	Device_ftm80_base::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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

		if (index > 10)
		{
			TRACE_ERROR("Invalid sensro id!");
			return	false;
		}
		
		_time = time_;
		TRACE_INFO(" DATA READ TIME : " << time_);
		switch((FTM80_OID_DATA[index].oid_ >> 24) & 0xff)
		{
			case 0x01 :
			case 0x02 :
				{
					double _data = FTM80_OID_DATA[index].data_ * 0.01;
					_value = ToString(_data, 2);
					break;
				}
			case 0x05 :
			case 0x06 :
				{
					_value = ToString((bool)FTM80_OID_DATA[index].data_);
					break;
				}
			case 0x07 :
				{
					_value = ToString(FTM80_OID_DATA[index].data_);
					break;
				}

		}
		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	Device_ftm80_base::Preprocess()
{
	DeviceU2U::Preprocess();
}


void	Device_ftm80_base::Process()
{
	float p = 0;
	uint8_t Req_msg[13] = {0x00};
	uint8_t Resp_msg[HID_UART_MAX_READ_SIZE];
	uint32_t Resp_msg_len;
	
	if(correction_timer_.RemainTime() == 0)
	{
		for(uint16_t i = 0; i < 10; i++)
		{
		TRACE_INFO("FTM80_BASE read data");
		memset(Resp_msg, 0x00,HID_UART_MAX_READ_SIZE);
		if (!RequestandWait(Req_msg, sizeof(Req_msg), Resp_msg, 12, Resp_msg_len, 2))
		{
			TRACE_ERROR("Failed to read register");
			//time_ = Date::GetCurrent();
			//for(uint16_t i = 0 ; i < 10 ; i++)
			//{
			//	FTM80_OID_DATA[i].data_ = 0;
			//}
		}
		else
		{
			if((Resp_msg[Resp_msg_len-1] << 8 | Resp_msg[Resp_msg_len-2]) == crc16(Resp_msg, Resp_msg_len-2))
			{
				uint8_t index = (Resp_msg[5] & 0xff) - 1;
				
				uint32_t oid_ = (Resp_msg[2] << 24) | (Resp_msg[3] << 16) | (Resp_msg[4] << 8) | Resp_msg[5];
				FTM80_OID_DATA[index].oid_ = oid_;
				int data_ = (Resp_msg[6] << 24) | (Resp_msg[7] << 16) | (Resp_msg[8] << 8) | Resp_msg[9];
				FTM80_OID_DATA[index].data_ = data_;
				//TRACE_INFO("Success to save index : " << index);	
				//TRACE_INFO("Success to read oid : " << oid_);
				//TRACE_INFO("Success to read data : " << data_);
			}	
			//TRACE_INFO("Success to read oid :" << FTM80_OID_DATA[index].oid_);
			//TRACE_INFO("Success to read data :" << FTM80_OID_DATA[index].data_);
		}
		}
			
		time_ = Date::GetCurrent();
		TRACE_INFO(" DATA GET TIME : " << time_);
		correction_timer_.Add(correction_interval_);
		TRACE_INFO("FTM80_BASE Remain Time : " << correction_timer_.RemainTime());
	}

	DeviceU2U::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	Device_ftm80_base::Type()
{
	return	OBJECT_TYPE_DEV_FTM80_BASE;
}


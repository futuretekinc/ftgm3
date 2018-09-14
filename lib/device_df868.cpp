#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_df868.h"
#include "crc_df868.h"
#include <string.h>
#include <cstdlib>

static const char*	class_name = "DeviceDF868";

DeviceDF868::DeviceDF868(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_DF868, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceDF868::DeviceDF868(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_DF868, true), correction_interval_(1)
{
	TRACE_INFO("DF868 Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceDF868::GetClassName()
{
	return	class_name;
}

bool	DeviceDF868::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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

		if ((index > DF868_SENSOR_COUNT))
		{
			TRACE_ERROR("Invalid sensor id!");
			return	false;
		}
		_time = time_;	
		switch(index)
		{
			case 0 : 
				{
					double value_ = 0.0;
					value_ = data_[index] * 0.01;
					_value = ToString(value_, 2);
					break;
				}
			case 1 :
				{
					double value_ = 0.0;
					value_ = data_[index] * 0.1;
					_value = ToString(value_, 1);
					break;
				}
			case 2 :
				{
					_value = ToString(data_[index]);
					break;
				}
		}

		
		//_time = time_;
		//_value = ToString(data_[index]);
		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

std::string* DeviceDF868::strSplit(std::string strOrigin, std::string strTok, int size)
{
	int cutAt;
	int index = 0;
	std::string* strResult = new std::string[size];
	while((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if( cutAt > 0 )
		{
			strResult[index++] = strOrigin.substr(0,cutAt);
		}
		strOrigin = strOrigin.substr(cutAt + 1);
	}
	if(strOrigin.length() > 0)
	{
		strResult[index++] = strOrigin.substr(0,cutAt);
	}
	return strResult;
}

double DeviceDF868::cal_data(double mantissa, double exponent)
{
	double total_result = 0;
	double exponent_define = 0;
	double absolute_value = 0;

	total_result = mantissa;

	if(exponent == 0)
	{
		exponent_define = 1;
		absolute_value = 0;
	}
	else if(exponent > 0)
	{
		exponent_define = 10;
		absolute_value = exponent;
	}
	else
	{
		exponent_define = 0.1;
		absolute_value = exponent * -1.0;
	}

	
	for(uint8_t i = 0; i < absolute_value ; i++)
	{
		total_result = total_result * exponent_define;
	}
	return total_result;

}

void	DeviceDF868::Preprocess()
{
	DeviceModbus::Preprocess();
}

void	DeviceDF868::Process()
{
	uint8_t slave_id = 0;
	float p = 0;
	uint8_t Req_msg[6] = {0x00};
	uint8_t Resp_msg[14];
	uint32_t Resp_msg_len;
	std::string split_char = "Em/sGJmA";
	std::string* temp;
	size_t ex_size = 0;	
	uint8_t serial_command[DF868_SENSOR_COUNT][12] = {
		{0x10,0x02,0x01,0x00,0x50,0x01,0x00,0x00,0x00,0x00,0x10,0x03}, 
		{0x10,0x02,0x01,0x00,0x50,0x01,0x00,0x01,0x00,0x00,0x10,0x03}, 
		{0x10,0x02,0x01,0x00,0x50,0x01,0x00,0x02,0x00,0x00,0x10,0x03} 
	};
	uint16_t df868_crc = 0;


	slave_id = atoi(Node::GetDevID().c_str());

	
	if(correction_timer_.RemainTime() == 0)
	{

		TRACE_INFO("DF868 read data");
#if 1
		for(uint8_t i = 0 ; i < DF868_SENSOR_COUNT ; i++)
		{
			for(uint8_t retry = 0; retry < 3 ; retry++)
			{

				memset(Resp_msg, 0x00, sizeof(Resp_msg));
				Resp_msg_len = 0;
				serial_command[i][3] = slave_id;
			
				df868_crc = Compute_CRC16(&serial_command[i][2],6);
				serial_command[i][8] = ((df868_crc >> 8) & 0xff);
				serial_command[i][9] = (df868_crc & 0xff);

				if(!RequestAndWait(serial_command[i], sizeof(serial_command[i]), Resp_msg, sizeof(Resp_msg), Resp_msg_len, 2 ))
				{
					TRACE_ERROR("Failed to read data");
					time_ = Date::GetCurrent();
					data_[i] = 0;
					continue;
				}
				else
				{
					time_ = Date::GetCurrent();
					df868_crc = Compute_CRC16(&Resp_msg[2],8);
					if(df868_crc != ((Resp_msg[10] << 8) | Resp_msg[11]))
					{
						TRACE_ERROR(" Command [" << i+1 << "] CRC IS ERROR" << df868_crc << ":" << ((Resp_msg[10] << 8) | Resp_msg[11]));
						data_[i] = 0;
						continue;
					}
					else
					{
						data_[i] = (Resp_msg[6] << 24) | (Resp_msg[7] << 16) | (Resp_msg[8] << 8) | Resp_msg[9] ;
					}
					TRACE_INFO("Success to Read data [" << i+1 <<"] : " << data_[i]);
					break;
				}
			}
		}
#else
		for(uint8_t i = 0 ; i < DF868_SENSOR_COUNT ; i++)
		{
			if(1)
			{
				time_ = Date::GetCurrent();
				data_[i] = 123+i;
			}
			else
			{
				data_[i] = 0;
			}
		}
#endif
		correction_timer_.Add(correction_interval_);

		TRACE_INFO("DF868 Remain Time : " << correction_timer_.RemainTime());

	}
	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceDF868::Type()
{
	return	OBJECT_TYPE_DEV_DF868;
}


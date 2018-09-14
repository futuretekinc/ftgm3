#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_sonic_205.h"
#include <string.h>
#include <cstdlib>

static const char*	class_name = "DeviceSONIC205";

DeviceSONIC205::DeviceSONIC205(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_SONIC205, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceSONIC205::DeviceSONIC205(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_SONIC205, true), correction_interval_(1)
{
	TRACE_INFO("SONIC205 Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceSONIC205::GetClassName()
{
	return	class_name;
}

bool	DeviceSONIC205::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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

		if ((index > SONIC_205_SENSOR_COUNT))
		{
			TRACE_ERROR("Invalid sensor id!");
			return	false;
		}

		
		_time = time_;
		_value = ToString(data_[index], 4);
		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

std::string* DeviceSONIC205::strSplit(std::string strOrigin, std::string strTok, int size)
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

double DeviceSONIC205::cal_data(double mantissa, double exponent)
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

void	DeviceSONIC205::Preprocess()
{
	DeviceModbus::Preprocess();
}

uint32_t total = 0;
uint32_t suc = 0;
uint32_t fail = 0;

void	DeviceSONIC205::Process()
{
	float p = 0;
	uint8_t Req_msg[6] = {0x00};
	uint8_t Resp_msg[30];
	uint32_t Resp_msg_len;
	std::string split_char = "Em/sGJmA";
	std::string* temp;
	size_t ex_size = 0;	
	uint8_t serial_command[SONIC_205_SENSOR_COUNT][6] = {
		{0x44,0x51,0x48,0x0d,0x0a,0x00}, //DQH - instantaneous flow per HOUR
		{0x44,0x56,0x0d,0x0a,0x00},      //Dv  - instantaneous velocity
		{0x44,0x49,0x4e,0x0d,0x0a,0x00}, //DIN - net inctrement
	};

	if(correction_timer_.RemainTime() == 0)
	{

		TRACE_INFO("SONIC205 read data");

		for(uint8_t i = 0 ; i < SONIC_205_SENSOR_COUNT ; i++)
		{
			total++;
			for(uint8_t retry = 0; retry < 3; retry++)
			{
				memset(Resp_msg, 0x00, sizeof(Resp_msg));
				if(!RequestAndResponse(serial_command[i], strlen((const char*)serial_command[i]), Resp_msg, sizeof(Resp_msg), Resp_msg_len, 10 ))
				{
					TRACE_ERROR("Failed to read data");
					time_ = Date::GetCurrent();
					data_[i] = 0;
					fail++;
					continue;
				}
				else
				{
					if((Resp_msg_len == 0))
					{
						TRACE_ERROR(" READ PACKET LENGTH ERROR (" << i <<")");
						time_ = Date::GetCurrent();
						data_[i] = 0;
						fail++;
						continue;
					}
					time_ = Date::GetCurrent();
					std::string _data(reinterpret_cast<char *>(Resp_msg));
					temp = strSplit(_data, split_char, 6);
					data_[i] = cal_data(atof(temp[0].c_str()),atof(temp[1].c_str()));
					suc++;
					break;
				}
			}
		}
		TRACE_INFO(" sinroisung : " << total <<"   "<< suc <<"   "<<fail);
		correction_timer_.Add(correction_interval_);

		TRACE_INFO("SONIC205 Remain Time : " << correction_timer_.RemainTime());

	}
	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceSONIC205::Type()
{
	return	OBJECT_TYPE_DEV_SONIC205;
}


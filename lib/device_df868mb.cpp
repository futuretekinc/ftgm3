#include <math.h>
#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_df868mb.h"


static const char*	class_name = "DeviceDF868MB";

DeviceDF868MB::DeviceDF868MB(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_DF868MB, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceDF868MB::DeviceDF868MB(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_DF868MB, true), correction_interval_(1)
{
	TRACE_INFO("DF868MB Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceDF868MB::GetClassName()
{
	return	class_name;
}

bool	DeviceDF868MB::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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


		_time = time_;
		_value = ToString(d_data_[index],4);
		

		return	true;
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR(e.what());
		return	false;	
	}
}

void	DeviceDF868MB::Preprocess()
{
	DeviceModbus::Preprocess();
}

double DeviceDF868MB::Binary32ToDouble(int value) // IEEE integer convert double.
{
	int minus = -1, exponent;
	double fraction, result;
	     
	if((value&0x80000000) == 0) minus = 1;
	exponent = ((value & 0x7F800000) >> 23) - 127;
	fraction = (value & 0x7FFFFF) + 0x800000;
	fraction = fraction / 0x800000;
	result = minus * fraction * pow(2, exponent);
	return(result);
}

void	DeviceDF868MB::Process()
{
	float p = 0;
	double d_velocity = 0.0;
	int i_velocity = 0;
	double d_volumetric = 0.0;
	int i_volumetric = 0;
	double d_total_pos = 0.0;
	int i_total_pos = 0;
	uint32_t slave_id = 0;

	slave_id = atoi(Node::GetDevID().c_str());
	if(slave_id == 0)
	{
		TRACE_ERROR("Not Set Slave ID");
	}
	else
	{
		if(correction_timer_.RemainTime() == 0)
		{
			for(uint8_t retry = 0 ; retry < 3 ; retry++)
			{
				TRACE_INFO("DF868MB read register");
				if (!ReadHoldingRegisters(slave_id, 0, registers_, 10))
				{
					TRACE_ERROR("Failed to read register");
					time_ = Date::GetCurrent();
					registers_[0] = 0; // clear totalizers
					registers_[1] = 0; // CH1 Velocity MSB
					registers_[2] = 0; // CH1 Velocity LSB (scaling 2)
					registers_[3] = 0; // CH1 Volumetric MSB
					registers_[4] = 0; // CH1 Volumetric LSB(IEEE 32bit)
					registers_[5] = 0; // CH1 +Totals MSB
					registers_[6] = 0; // CH1 +Totals LSB
					registers_[7] = 0; // CH1 -Totals MSB
					registers_[8] = 0; // Ch1 -Totals LSB
					registers_[9] = 0; // CH1 #T Digits( totals scaling )
					d_data_[0] = 0;
					d_data_[1] = 0;
					d_data_[2] = 0;
					continue;
				}
				else
				{
					/////veolcity calculation////
					i_velocity = (int)((registers_[1] << 16) | registers_[2]);
					d_velocity = i_velocity * 0.01;
					d_data_[0] = d_velocity;
					//s_data_[0] = ToString(d_velocity, 4);	
			
					TRACE_INFO(" VELOCITY :" << d_velocity);

					/////volumetric calculation////
					i_volumetric = (int)((registers_[3] << 16) | registers_[4]);
					d_volumetric = Binary32ToDouble(i_volumetric);
					d_data_[1] = d_volumetric;
					//s_data_[1] = ToString(d_volumetric, 4);	
			

					TRACE_INFO(" VOLUMETRIC :" << d_volumetric);

					////total positive calculation////
					i_total_pos = (int)((registers_[5] << 16) | registers_[6]);
					d_total_pos = i_total_pos;
			
					for(uint16_t i = 0 ; i < registers_[9] ; i++)
					{
						d_total_pos = d_total_pos * 0.1;
					}
					d_data_[2] = d_total_pos;
					//s_data_[2] = ToString(d_total_pos, 4);	
					time_ = Date::GetCurrent();
			
					TRACE_INFO(" TOTAL POS :" << d_total_pos);
					break;
				}
			}
		

			correction_timer_.Add(correction_interval_);
			TRACE_INFO("DF868MB Remain Time : " << correction_timer_.RemainTime());
		}
	}

	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceDF868MB::Type()
{
	return	OBJECT_TYPE_DEV_DF868MB;
}


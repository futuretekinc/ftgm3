#include <math.h>
#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_sonic_205mb.h"


static const char*	class_name = "DeviceSONIC205MB";

DeviceSONIC205MB::DeviceSONIC205MB(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_SONIC205MB, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceSONIC205MB::DeviceSONIC205MB(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_SONIC205MB, true), correction_interval_(1)
{
	TRACE_INFO("SONIC205MB Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceSONIC205MB::GetClassName()
{
	return	class_name;
}

bool	DeviceSONIC205MB::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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
		
		if(index > 2)
		{
			TRACE_ERROR(" Sensor ID is false");
		}

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

void	DeviceSONIC205MB::Preprocess()
{
	DeviceModbus::Preprocess();
}

double DeviceSONIC205MB::Binary32ToDouble(int value) // IEEE integer convert double.
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

double DeviceSONIC205MB::cal_data(uint32_t  mantissa,  int16_t exponent)
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

void	DeviceSONIC205MB::Process()
{
	float p = 0;
	double d_velocity = 0.0;
	int32_t i_velocity = 0;
	double d_frpm = 0.0;
	int i_frpm = 0;
	double d_net_total = 0.0;
	uint32_t ui_net_total = 0;
	uint16_t slave_id = 0;
	uint16_t msb, lsb;
	int16_t exponent;
	slave_id = atoi(Node::GetDevID().c_str());

	if(slave_id == 0)
	{
		TRACE_ERROR(" NOT SET SLAVE ID ");
	}
	else
	{
	
		if(correction_timer_.RemainTime() == 0)
		{
			for(uint8_t retry = 0 ; retry < 3; retry++)
			{
				TRACE_INFO("SONIC205MB read register");
				if (!ReadHoldingRegisters(slave_id, 0, registers_, 17))
				{

					TRACE_ERROR("Failed to read register");
					time_ = Date::GetCurrent();
					registers_[0] = 0; // Flow Rate Per Second LSB
					registers_[1] = 0; // Flow Rate Per Second MSB
					registers_[2] = 0; // Flow Rate Per Minute LSB
					registers_[3] = 0; // Flow Rate Per Minute MSB
					registers_[4] = 0; // Flow Rate Per Hour LSB
					registers_[5] = 0; // Flow Rate Per Hour MSB
					registers_[6] = 0; // Velocity LSB
					registers_[7] = 0; // Velocity MSB
					registers_[8] = 0; // PT LSB
					registers_[9] = 0; // PT MSB
					registers_[10] = 0; // PT Exponent
					registers_[11] = 0; // NT LSB
					registers_[12] = 0; // NT MSB
					registers_[13] = 0; // NT Exponent
					registers_[14] = 0; // Net Total LSB
					registers_[15] = 0; // Net Total MSB
					registers_[16] = 0; // Net Exponent
					d_data_[0] = 0;
			        	d_data_[1] = 0;
			 		d_data_[2] = 0;
					continue;
				}	
				else
				{
					/////Flow Rate Per Minute  calculation////
					msb = registers_[5];
					lsb = registers_[4];
					i_frpm = (int)((msb << 16) | lsb);
					d_frpm= Binary32ToDouble(i_frpm);
					d_data_[0] = d_frpm;
					//s_data_[0] = ToString(d_frpm, 4);	
			
					TRACE_INFO(" flow rate/m : " << d_data_[0]);

					/////velocity calculation////
					msb = registers_[7];
					lsb = registers_[6];
					i_velocity = (int)((msb << 16) | lsb);
					d_velocity = Binary32ToDouble(i_velocity);
					d_data_[1] = d_velocity;
					//s_data_[1] = ToString(d_velocity, 4);	

					TRACE_INFO(" VELOCITY : " << d_data_[1]);

					////Net Total calculation////
					msb = registers_[15];
					lsb = registers_[14];
					exponent = registers_[16];
					ui_net_total = (uint32_t)((msb << 16) | lsb);
					d_net_total = cal_data(ui_net_total, exponent);
					d_data_[2] = d_net_total;	
					//s_data_[2] = ToString(d_net_total, 4);	
					time_ = Date::GetCurrent();
			
					TRACE_INFO(" Net Total : " << d_data_[2]);
					break;
				}
			}

			correction_timer_.Add(correction_interval_);
			TRACE_INFO("SONIC205MB Remain Time : " << correction_timer_.RemainTime());
		}
	}

	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceSONIC205MB::Type()
{
	return	OBJECT_TYPE_DEV_SONIC205MB;
}


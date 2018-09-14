#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_a3300.h"
#include <string.h>
#include <cstdlib>
#include <math.h>
static const char*	class_name = "DeviceA3300";

#define	BTOF 1 

DeviceA3300::DeviceA3300(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_A3300, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);
}

DeviceA3300::DeviceA3300(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_A3300, true), correction_interval_(1)
{
	TRACE_INFO("A3300 Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("even");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceA3300::GetClassName()
{
	return	class_name;
}

bool	DeviceA3300::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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

		if ((index > A3300_SENSOR_COUNT))
		{
			TRACE_ERROR("Invalid sensor id!");
			return	false;
		}
		_time = time_;
		if(index == 10)
		{
			_value = ToString(data_[index],4);
		}	
		else
		{
			_value = ToString(data_[index],2);
		}
//		_value = ToString(data_[index]);
	//	_value = data_[index];	
		//sensor id info
		//sensor id 1 : voltage a
		//sensor id 2 : voltage b
		//sensor id 3 : voltage c
		//sensor id 4 : current a
		//sensor id 5 : current b
		//sensor id 6 : current c
		//sensor id 7 : power   a
		//sensor id 8 : power   b
		//sensor id 9 : power   c
		//sensor id 10 : total power	
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


double DeviceA3300::Binary32ToDouble(int value) // IEEE integer convert double.
{
	int minus = -1, exponent;
	double fraction, result;

       if((value&0x80000000) == 0)
       {
	       minus = 1;
       }
     	TRACE_INFO("VALUE : " << ToString(value));  
       exponent = ((value & 0x7F800000) >> 23) - 127;
       fraction = (value & 0x7FFFFF) + 0x800000;
       fraction = fraction / 0x800000;
       TRACE_INFO("MINUS : " << ToString(minus));
       TRACE_INFO("EXPONENT : " << ToString(exponent));
       TRACE_INFO("FRACTOPM "  << ToString(fraction, 2));
       result = minus * fraction * pow(2, exponent);
       return result;
}

void	DeviceA3300::Preprocess()
{
	Date date;
	correction_interval_ = GetCorrectionInterval();
	TRACE_INFO("A3300 DEVICE CORRECTION TIME : " << ToString(correction_interval_));
	date = Date::GetCurrent() + Time(correction_interval_ * TIME_SECOND);
	correction_timer_.Set(date);
	DeviceModbus::Preprocess();
}

double  DeviceA3300::convertFloat(int16_t* reg_temp)
{
	int temp = 0;
	temp = (reg_temp[0] << 16) | reg_temp[1];
	return Binary32ToDouble(temp);

}

void	DeviceA3300::Process()
{
	uint32_t slave_id = 0;
	
	uint16_t a3300_crc = 0;
	int16_t  voltage_registers[10];
	int16_t	 current_registers[10];
	int16_t	 power_registers[10];
	int16_t	 total_power_registers[10];
	int16_t	 registers_[28];
	
	uint8_t  index = 0;

	slave_id = atoi(Node::GetDevID().c_str());
	if(slave_id == 0)
	{
		TRACE_ERROR("Not Set Slave ID");
	}	
	else
	{
		if(correction_timer_.RemainTime() == 0)
		{

			TRACE_INFO("A3300 read data");
			uint8_t retry = 0;
			for( ; retry < 3 ; retry++)
			{
				TRACE_INFO("A3300 voltage Read");
#if (BTOF == 0)	
				if(!ReadHoldingRegisters(slave_id, 100, voltage_registers ,9))
				{
					continue;	
				}
				else
				{
					uint16_t voltage = 0;
					uint16_t scale = (uint16_t)voltage_registers[8];
					for(uint8_t i = 0 ; i < 3 ; i++)
					{
						voltage = (uint16_t)voltage_registers[i];
						data_[i] = voltage * scale * 0.1;
					}	
				}
#else
				if(!ReadHoldingRegisters(slave_id, 9000, voltage_registers ,6))
				{
					continue;
				}	
				else
				{
					int i_temp = 0;
					double d_temp = 0.0;

				       	i_temp = (int)(((uint16_t)voltage_registers[0] << 16) | (uint16_t)voltage_registers[1]);
				       	TRACE_INFO(" I_TEMP : " << ToString(i_temp));	
					data_[0] = Binary32ToDouble(i_temp);
					
										
				       	i_temp = (int)(((uint16_t)voltage_registers[2] << 16) | (uint16_t)voltage_registers[3]); 
				
				       	TRACE_INFO(" I_TEMP : " << ToString(i_temp));	
					data_[1] = Binary32ToDouble(i_temp);
					
				       	
					i_temp = (int)(((uint16_t)voltage_registers[4] << 16) | (uint16_t)voltage_registers[5]); 
					
				       	TRACE_INFO(" I_TEMP : " << ToString(i_temp));	
					data_[2] = Binary32ToDouble(i_temp);
					
					break;	
				}
#endif					
			}

			retry = 0;
			for(; retry < 3 ; retry++)
			{
				TRACE_INFO("A3300 current Read");
			
#if ( BTOF == 0 )	
				if(!ReadHoldingRegisters(slave_id, 109 , current_registers, 9))
				{
					continue;	
				}
				else
				{
					uint16_t current = 0;
					uint16_t scale = (uint16_t)current_registers[8];
					for(uint8_t i = 0 ; i < 3 ; i++)
					{
						current = (uint16_t)current_registers[i];
						data_[i+3] = current * scale * 0.001;
					}	
				}
#else
				
				if(!ReadHoldingRegisters(slave_id, 9012 , current_registers, 6))
				{
					continue;
				}
				else
				{
					int i_temp = 0;
					double d_temp = 0.0;

				       	i_temp =  (int)(((uint16_t)current_registers[0]) << 16 | (uint16_t)current_registers[1]); 
					data_[3] = Binary32ToDouble(i_temp);
										
				       	i_temp =  (int)(((uint16_t)current_registers[2]) << 16 | (uint16_t)current_registers[3]); 
					data_[4] = Binary32ToDouble(i_temp);
				       	
					i_temp = (int)(((uint16_t)current_registers[4]) << 16 | (uint16_t)current_registers[5]); 
					data_[5] = Binary32ToDouble(i_temp);
					
					break;	
				}
#endif					
			}

			retry = 0;	
			for( ; retry < 3 ; retry++)
			{
				TRACE_INFO("A3300 power Read");
#if ( BTOF == 0 )	
				if(!ReadHoldingRegisters(slave_id, 118, power_registers, 4))
				{
					continue;	
				}
				else
				{
					int16_t power = 0;
					uint16_t scale = (uint16_t)power_registers[3];
					for(uint8_t i = 0 ; i < 3 ; i++)
					{
						power = power_registers[i];
						data_[i+6] = power * scale * 0.001;
					}	
				}
#else
				if(!ReadHoldingRegisters(slave_id, 9020, power_registers, 4))
				{
					continue;
				}	
				else
				{
					int i_temp = 0;
				       	double d_temp = 0.0;
					i_temp =(int)(((uint16_t)power_registers[0]) << 16 | (uint16_t)power_registers[1]); 
					data_[6] = Binary32ToDouble(i_temp);
					
					i_temp =(int)(((uint16_t)power_registers[2] << 16) | (uint16_t)power_registers[3]); 
					data_[7] = Binary32ToDouble(i_temp);
				       	
					i_temp =(int)(((uint16_t)power_registers[4] << 16) | (uint16_t)power_registers[5]); 
					data_[8] = Binary32ToDouble(i_temp);
					
					break;	
				}
#endif					
			}
			retry = 0;
			for(; retry < 3 ; retry++)
			{
				TRACE_INFO("A3300 total power Read");
#if ( BTOF == 0 )	
				if(!ReadHoldingRegisters(slave_id, 122 ,total_power_registers ,2))
				{
					continue;	
				}
				else
				{
					int16_t total_power = total_power_registers[0];
					uint16_t scale = (uint16_t)total_power_registers[1];
					data_[9] = total_power * scale * 0.001;
				}
#else		
				if(!ReadHoldingRegisters(slave_id, 9026 ,total_power_registers ,6))
				{
					continue;
				}	
				else
				{
					int i_temp = 0;
					double d_temp = 0;

				       	i_temp =  (int)(((uint16_t)total_power_registers[0] << 16) | (uint16_t)total_power_registers[1]); 
					data_[9] = Binary32ToDouble(i_temp);
					
					break;	
				}
#endif					
			}
			retry = 0;
			for(;retry < 3; retry++)
			{
				TRACE_INFO("A3300 total kwh");
				if(!ReadHoldingRegisters(slave_id, 9049, total_power_registers, 2))
				{
					continue;
				}
				else
				{
					int i_temp = 0;
					i_temp = (int)(((uint16_t)total_power_registers[0] << 16) | (uint16_t)total_power_registers[1]);
					data_[10] = (double)(i_temp * 0.0001);
					break;
				}
			}
			/*for(uint8_t retry = 0 ; retry < 3 ; retry++)
			{
				TRACE_INFO("A3300 MODBUS READ : " << ToString(retry));
				if(!ReadHoldingRegisters(slave_id, 0, registers_, 28))
				{
					continue;
				}
				else
				{
					data_[0] = convertFloat(registers_);     //Voltage a
					data_[1] = convertFloat(registers_+2);   //Voltage b
					data_[2] = convertFloat(registers_+4);   //Voltage c
					data_[3] = convertFloat(registers_+12);  //Current a
					data_[4] = convertFloat(registers_+14);  //Current b
					data_[5] = convertFloat(registers_+16);  //Current c
					data_[6] = convertFloat(registers_+20);  //Power   a
					data_[7] = convertFloat(registers_+22);  //Power   b
					data_[8] = convertFloat(registers_+24);  //Power   c
					data_[9] = convertFloat(registers_+26);  //TotalPower
				}
			}*/	
			time_ = Date::GetCurrent();
		
			correction_timer_.Add(correction_interval_);

			TRACE_INFO("A3300 Remain Time : " << correction_timer_.RemainTime());
		}

	}
	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceA3300::Type()
{
	return	OBJECT_TYPE_DEV_A3300;
}


#include "defined.h"
#include "exception.h"
#include "utils.h"
#include "object_manager.h"
#include "device_test.h"
#include <string.h>
#include <cstdlib>
#include <math.h>
static const char*	class_name = "DeviceTEST";


DeviceTEST::DeviceTEST(ObjectManager& _manager)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_TEST, true), correction_interval_(1)
{
	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);
}

DeviceTEST::DeviceTEST(ObjectManager& _manager, JSONNode const& _properties)
: DeviceModbus(_manager, OBJECT_TYPE_DEV_TEST, true), correction_interval_(1)
{
	TRACE_INFO("TEST Created");

	serial_.SetBaudrate(9600);
	serial_.SetParityBit("none");
	serial_.SetDataBit(8);

	SetProperties(_properties, false, true);
}

const char*	DeviceTEST::GetClassName()
{
	return	class_name;
}

bool	DeviceTEST::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
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

		if ((index > TEST_SENSOR_COUNT))
		{
			TRACE_ERROR("Invalid sensor id!");
			return	false;
		}
		_time = time_;
		_value = ToString(data_[index],2);
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


double DeviceTEST::Binary32ToDouble(int value) // IEEE integer convert double.
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

void	DeviceTEST::Preprocess()
{
	Date date;
	correction_interval_ = GetCorrectionInterval();
	TRACE_INFO("TEST DEVICE CORRECTION TIME : " << ToString(correction_interval_));
	date = Date::GetCurrent() + Time(correction_interval_ * TIME_SECOND);
	correction_timer_.Set(date);
	DeviceModbus::Preprocess();
}

double  DeviceTEST::convertFloat(int16_t* reg_temp)
{
	int temp = 0;
	temp = (reg_temp[0] << 16) | reg_temp[1];
	return Binary32ToDouble(temp);

}

void	DeviceTEST::Process()
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
			for(uint8_t i ; i < TEST_SENSOR_COUNT ; i++)
			{
				data_[i] = (rand() % 1000) * 0.01;
			}

			time_ = Date::GetCurrent();
		
			correction_timer_.Add(correction_interval_);

			TRACE_INFO("TEST Remain Time : " << correction_timer_.RemainTime());
		}

	}
	DeviceModbus::Process();
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	DeviceTEST::Type()
{
	return	OBJECT_TYPE_DEV_TEST;
}


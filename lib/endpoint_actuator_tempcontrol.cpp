#include "defined.h"
#include "exception.h"
#include "json.h"
#include "json_utils.h"
#include "object_manager.h"
#include "device.h"
#include "utils.h"
#include "endpoint_actuator_tempcontrol.h"

static const char* class_name = "EPAtempcontrol";

EndpointActuatorTempControl::EndpointActuatorTempControl(ObjectManager& _manager, std::string const& _type, std::string const& unit, double _min, double _max)
: EndpointActuator(_manager, _type), target_value(0), heating_value(0), cooling_value(0), value_min_(_min), value_max_(_max)
{
	trace.SetClassName(class_name);
}

EndpointActuatorTempControl::EndpointActuatorTempControl(ObjectManager& _manager, std::string const& _type,  JSONNode const& _properties, std::string const& unit, double _min, double _max)
: EndpointActuator(_manager, _type), target_value(0), heating_value(0), cooling_value(0), value_min_(_min), value_max_(_max)
{
	trace.SetClassName(class_name);
	SetProperties(_properties , false, true);

}

const char*	EndpointActuatorTempControl::GetClassName()
{
	return	class_name;
}

bool	EndpointActuatorTempControl::IsValid(std::string const& _value)
{
	double value = strtod(_value.c_str(), NULL);

	return ((value_min_ <= value) && (value <= value_max_));
}

std::string* EndpointActuatorTempControl::strSplit(std::string strOrigin, std::string strTok, int size)
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

std::string	EndpointActuatorTempControl::GetValue()
{
	//return	ToString(value_, 2);
	//std::string result_value = ToString(target_value,2) + "," + ToString(heating_value,2) + "," + ToString(cooling_value,2);
	Device* device = manager_.GetDevice(parent_id_);
	if (device == NULL)
	{
		TRACE_ERROR("Device not found!");
		return  false;
	}
	time_t time;
	std::string value_temp;
	device->ReadValue(id_, time, value_temp);
	TRACE_INFO(" CONTROL READ DATA : " << value_temp);
	std::string etc = ",";
	std::string* rev_value = strSplit(value_temp, etc, 3);


	save_value = rev_value[0] + "," + rev_value[1] + "," + rev_value[2];
	return	save_value;
}

bool	EndpointActuatorTempControl::SetValue(std::string const& _value, bool _check)
{

	std::string* str = strSplit(_value, ",", 3);

	

	if (!IsValid(str[0]))
	{
		TRACE_ERROR("Invalid value[" << str[0]<< "]");
		return	false;	
	}

	if (!_check)
	{
		double d_target_value_tmp  = strtod(str[0].c_str(), NULL);
		double d_cooling_value_tmp  = strtod(str[1].c_str(), NULL);
		double d_heating_value_tmp  = strtod(str[2].c_str(), NULL);

		if ((target_value != d_target_value_tmp) || (heating_value != d_heating_value_tmp) || (cooling_value != d_cooling_value_tmp))
		{
			Device*	device = manager_.GetDevice(parent_id_);
			if (device == NULL)
			{
				TRACE_ERROR("Device not found!");
				return	false;
			}

		        std::string s_target_value = "\"targetTemperature\":";
			std::string s_heating_value = "\"heatingDeviation\":";
			std::string s_cooling_value = "\"coolingDeviation\":";
			std::string s_temp = "\"";
			

			//s_target_value = s_target_value + s_temp + str[0] + s_temp + ",";
			//s_cooling_value = s_cooling_value + s_temp + str[1] + s_temp +",";
			//s_heating_value = s_heating_value + s_temp + str[2] + s_temp ;
		
				
			s_target_value = s_target_value + str[0] + ",";
			s_cooling_value = s_cooling_value + str[1] +",";
			s_heating_value = s_heating_value + str[2] ;
			
			total_value = "{" + s_target_value + s_cooling_value + s_heating_value + "}";

			if (!device->WriteValue(id_, total_value))
			{
				TRACE_ERROR("Failed to write value to device!");
				return	false;
			}
			TRACE_INFO(" CONTROL WRITE VALUE : " << total_value);
			time_t	time;
			std::string value_temp;

			device->ReadValue(id_, time, value_temp);
			std::string etc = "\"{}:,";
			std::string* rev_value = strSplit(value_temp, etc, 6);
			
			TRACE_INFO("CONTROL READ VALUE 2 : " << value_temp);	
			
			if(rev_value[0].compare("T") == 0)
			{
				target_value = strtod(rev_value[1].c_str(), NULL);
				
			}
			else
			{
				return false;
			}
			
			if(rev_value[2].compare("C") == 0)
			{
				cooling_value = strtod(rev_value[3].c_str(), NULL);
			}
			else
			{
				return false;
			}
			
			if(rev_value[4].compare("H") == 0)
			{
				heating_value = strtod(rev_value[5].c_str(), NULL);
			}
			else
			{
				return false;
			}
			save_value = rev_value[1] + "," + rev_value[3] + "," + rev_value[5];

			Add(time, save_value);
		}
	}
	return	true;
}

std::string	EndpointActuatorTempControl::GetValueMin()
{
	return	ToString(value_min_, 2);
}

bool	EndpointActuatorTempControl::SetValueMin(std::string const& _value, bool _check)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	return	true;
}

std::string	EndpointActuatorTempControl::GetValueMax()
{
	return	ToString(value_max_, 2);
}

bool	EndpointActuatorTempControl::SetValueMax(std::string const& _value, bool _check)
{
	if (!IsValid(_value))
	{
		TRACE_ERROR("Invalid value[" << _value << "]");
		return	false;	
	}

	return	true;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char* EndpointActuatorTempControl::Type()
{
	return	OBJECT_TYPE_EP_A_TEMPCONTROL;
}


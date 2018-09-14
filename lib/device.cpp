#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "exception.h"
#include "json.h"
#include "trace.h"
#include "device.h"
#include "object_manager.h"
#include "device_snmp.h"
#include "device_modbus_tcp.h"
#include "device_adam_6051.h"
#include "device_turbomax.h"
#include "device_fte.h"
#include "device_sim.h"
#include "device_gtc_520a.h"
#include "device_hs_1000m.h"
#include "device_mb7092.h"
#include "device_nbiotm.h"
#include "device_a3300.h"
#include "device_ftm80_base.h"
#include "device_sonic_205.h"
#include "device_sonic_205mb.h"
#include "device_df868.h"
#include "device_df868_01.h"
#include "device_at868.h"
#include "device_df868mb.h"

#include "endpoint.h"
#include "endpoint_sensor.h"

Device::Device(ObjectManager& _manager, std::string const& _type)
:	Node(_manager, _type),correction_interval_(1)
{
}

Device::~Device()
{
}

Endpoint*	Device::CreateEndpoint(JSONNode const& _properties)
{
	Endpoint* endpoint = manager_.CreateEndpoint(_properties);
	if (endpoint != NULL)
	{
		Attach(endpoint->GetID());	
	}

	return	endpoint;
}


bool	Device::GetProperty(uint32_t _type, JSONNode& _property) 
{
	switch(_type)
	{
       		case    PROPERTY_CORRECTION_INTERVAL_FLAG:      _property = JSONNode(TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);     break;
	        default:
		{
			return  Node::GetProperty(_type, _property);
  		}
		break;
 	}
	return  true;
}


bool	Device::SetProperty(JSONNode const& _property, bool _check)
{
	bool ret_value = false;

	if ((_property.name() == TITLE_NAME_CORRECTION_INTERVAL) || (_property.name() == TITLE_NAME_UPDATE_INTERVAL))
	{
   		ret_value = SetCorrectionInterval(_property.as_string(), _check);
     	}
	else
	{
		ret_value = Node::SetProperty(_property, _check);
	}
	return ret_value;
}

 bool    Device::SetCorrectionInterval(uint32_t _interval)
{
  	correction_interval_ = _interval;
	JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);
	if (!lazy_store_)
  	{
    		ApplyChanges();
   	}
	return  true;
}
	
bool    Device::SetCorrectionInterval(std::string const& _interval, bool _check)
{
 	if (!_check)
   	{
 		correction_interval_ = strtoul(_interval.c_str(), NULL, 10);
 		JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);
		if (!lazy_store_)
 		{
  			ApplyChanges();
   		}
 	}
	return  true;
}
		
uint32_t        Device::GetCorrectionInterval()
{
	return  correction_interval_;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Device::IsAttached(std::string const& _epid)
{
	for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
	{
		if ((*it) == _epid)
		{	
			return	true;
		}
	}

	return	false;
}

bool	Device::Attach(std::string const& _epid)
{
	if (!IsAttached(_epid))
	{
		epid_list_.push_back(_epid);
		TRACE_INFO("The endpoint[" << _epid << "] has been attahced.");
	}

	return	true;
}

bool	Device::Detach(std::string const& _epid)
{
	for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
	{
		if ((*it) == _epid)
		{	
			epid_list_.erase(it);
			TRACE_INFO("The endpoint[" << _epid << "] has been detahced.");
			return	true;
		}
	}


	return	false;
}

bool	Device::Detach()
{
	epid_list_.clear();
	return	true;
}

uint32_t	Device::GetEndpointCount()
{
	return	epid_list_.size();
}

const std::string&	Device::GetEndpointAt(int index)
{
	if (index >= 0)
	{
		for(std::list<std::string>::iterator it = epid_list_.begin() ; it != epid_list_.end() ; it++)
		{
			if (index == 0)
			{	
				return	*it;
			}

			index--;
		}
	}

	static	std::string null_id("");
	return	null_id;
}

bool		Device::GetEndpointList(std::list<std::string>& _epid_list)
{
	_epid_list = epid_list_;

	return	true;
}

bool		Device::GetEndpointMap(std::map<std::string, Endpoint*>& _map)
{
	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
	{
		Endpoint* endpoint = manager_.GetEndpoint(*it);
		if (endpoint != NULL)
		{
			_map[*it] = endpoint;
		}
	}

	return	true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

Device::operator JSONNode()
{
	JSONNode	properties;

	Object::GetProperties(properties);

	return	properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Device::Preprocess()
{
	Node::Preprocess();

	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(*it);
		if (endpoint != NULL)
		{
			endpoint->Start();	
			usleep(100000);
		}
		else
		{
			TRACE_ERROR("Endpoint[" << (*it) << "] not found!");	
		}
	}
}

void	Device::Process()
{

	for(std::map<time_t, std::string>::iterator it = endpoint_schedule_list_.begin(); it != endpoint_schedule_list_.end() ; it++)
	{
		if (time_t(Date::GetCurrent()) >= it->first)
		{
			time_t		time = it->first;
			std::string	id 	= it->second;

			endpoint_schedule_list_.erase(it);

			Endpoint*	endpoint = manager_.GetEndpoint(id);
			if (endpoint != NULL)
			{
				endpoint->CorrectionProcess();

				time += ((time_t(Date::GetCurrent())  - time) / endpoint->GetCorrectionInterval() + 1) * endpoint->GetCorrectionInterval();

				AddSchedule(endpoint->GetID(), time);
			}

			break;
		}
	}

	Node::Process();
}

void	Device::Postprocess()
{
	for(std::list<std::string>::iterator it = epid_list_.begin(); it != epid_list_.end(); it++)
	{
		Endpoint*	endpoint = manager_.GetEndpoint(std::string(*it));
		if (endpoint != NULL)
		{
			endpoint->Stop();	
		}
	}

	Node::Postprocess();

}

bool	Device::AddSchedule(std::string const& _id, time_t _time)
{
	bool	last = true;

	if (!IsAttached(_id))
	{
		TRACE_INFO("The endpoint[" << _id << "] is not attached");
		if (!Attach(_id) )
		{
			TRACE_INFO("Failed to attach the endpoint[" << _id << "]");
			return	false;
		}
	}
	

	endpoint_schedule_list_lock_.Lock();

	endpoint_schedule_list_.insert(std::pair<time_t, std::string>(_time, _id));	

	endpoint_schedule_list_lock_.Unlock();

	return	true;
}

bool	Device::RemoveSchedule(std::string const& _id)
{
	bool	removed = false;
	endpoint_schedule_list_lock_.Lock();

	for(std::map<time_t, std::string>::iterator it = endpoint_schedule_list_.begin() ; it != endpoint_schedule_list_.end() ; it++)
	{
		if (it->second == _id)
		{
			endpoint_schedule_list_.erase(it);
			removed = true;
			break;
		}
	}

	endpoint_schedule_list_lock_.Unlock();

	return	removed;
}

Device*	Device::Create(ObjectManager& _manager, JSONNode const& _properties)
{
	Device*	device = NULL;
	try
	{
		TRACE_INFO2(&_manager, "< Create Device >" << std::endl << _properties.write_formatted());

		std::string	type = JSONNodeGetType(_properties);

		if (type == std::string(DeviceSNMP::Type()))
		{
			device = new DeviceSNMP(_manager, _properties);
		}
		else if (type == std::string(DeviceFTE::Type()))
		{
			device = new DeviceFTE(_manager, _properties);
		}
		else if (type == std::string(DeviceSIM::Type()))
		{
			device = new DeviceSIM(_manager, _properties);
		}
		else if (type == std::string(DeviceGTC520A::Type()))
		{
			device = new DeviceGTC520A(_manager, _properties);
		}
		else if (type == std::string(DeviceADAM6051::Type()))
		{
			device = new DeviceADAM6051(_manager, _properties);
		}
		else if(type == std::string(DeviceTurboMAX::Type()))
		{
			device = new DeviceTurboMAX(_manager, _properties);
		}
		else if(type == std::string(DeviceHS1000M::Type()))
		{
			device = new DeviceHS1000M(_manager, _properties);
		}
		else if(type == std::string(DeviceMB7092::Type()))
		{
			device = new DeviceMB7092(_manager, _properties);
		}
		else if(type == std::string(DeviceNBIOTM::Type()))
		{
			device = new DeviceNBIOTM(_manager, _properties);
		}	
		else if(type == std::string(DeviceA3300::Type()))
		{
			device = new DeviceA3300(_manager, _properties);
		}	
		else if(type == std::string(Device_ftm80_base::Type()))
		{
			device = new Device_ftm80_base(_manager, _properties);
		}
		else if(type == std::string(DeviceSONIC205::Type()))
		{
			device = new DeviceSONIC205(_manager, _properties);
		}
		else if(type == std::string(DeviceSONIC205MB::Type()))
		{
			device = new DeviceSONIC205MB(_manager, _properties);
    		}
 		else if(type == std::string(DeviceDF868::Type()))
		{
			device = new DeviceDF868(_manager, _properties);
		}
		else if(type == std::string(DeviceDF868MB::Type()))
		{
			device = new DeviceDF868MB(_manager, _properties);
		}
		else if(type == std::string(DeviceAT868::Type()))
		{
			device = new DeviceAT868(_manager, _properties);
		}
		else if(type == std::string(DeviceDF868_01::Type()))
		{
			device = new DeviceDF868_01(_manager, _properties);
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create device. Device type[" << type << "] is not supported!");
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR2(NULL, "Failed to create device. Device type unknown!");
	}
	catch(std::exception& e)
	{
		TRACE_ERROR2(&_manager, e.what());
	}

	return	device;
}


bool	Device::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Node::GetPropertyFieldList(_field_list))
	{
		_field_list.push_back(TITLE_NAME_CORRECTION_INTERVAL);
	}

	return	true;
}

bool	Device::ReadValue(std::string const& _epid, time_t& _time, std::string& _value)
{
	return	false;
}

bool	Device::ReadValue(std::string const& _epid, time_t& _time, uint32_t& _value)
{
	return	false;
}

bool	Device::ReadValue(std::string const& _epid, time_t& _time, bool& _value)
{
	return	false;
}

bool	Device::ReadValue(uint32_t _index, time_t& _time, bool& _value)
{
	return	false;
}

bool	Device::ReadValue(uint32_t _index, time_t& _time, uint32_t& _value)
{
	return	false;
}

bool	Device::ReadValue(uint32_t _index, time_t& _time, std::string& _value)
{
	return	false;
}

bool	Device::WriteValue(std::string const& _endpoint_id, std::string const& _value)
{
	TRACE_ENTRY;
	return	false;	
}

bool	Device::WriteValue(std::string const& _epid, uint32_t _value)
{
	TRACE_ENTRY;
	return	false;
}

bool	Device::WriteValue(std::string const& _endpoint_id, bool _value)
{
	TRACE_ENTRY;
	return	false;	
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	Device::Type()
{
	return	OBJECT_TYPE_DEVICE;
}


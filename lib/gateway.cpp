#include <strings.h>
#include <iomanip>
#include <list>
#include "defined.h"
#include "trace.h"
#include "node.h"
#include "gateway.h"
#include "device.h"
#include "gateway_gen.h"
#include "object_manager.h"
#include "utils.h"
#include "json.h"
#include "exception.h"

Gateway::Gateway(ObjectManager& _manager, std::string const& _type)
:	Node(_manager, _type),correction_interval_(ENDPOINT_REPORT_INTERVAL)
{
}

Gateway::~Gateway()
{
}

 bool    Gateway::SetCorrectionInterval(uint32_t _interval)
{
	correction_interval_ = _interval;
	JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);
	if (!lazy_store_)
	{
		ApplyChanges();
	}
	return  true;
}

 bool    Gateway::SetCorrectionInterval(std::string const& _interval, bool _check)
{
	if (!_check)
	{
		correction_interval_ = strtoul(_interval.c_str(), NULL, 10);
		manager_.SetEndpointReportInterval(correction_interval_);
		JSONNodeUpdate(updated_properties_, TITLE_NAME_CORRECTION_INTERVAL, correction_interval_);
		if (!lazy_store_)
   		{
 			ApplyChanges();
 		}
	}
	return  true;
}

bool	Gateway::SetProperty(JSONNode const& _property, bool _check)
{
	bool	ret_value = true;

	if (_property.name() == TITLE_NAME_DEVICE)
	{
	}
	else if(_property.name() == TITLE_NAME_CORRECTION_INTERVAL)
	{
		ret_value = SetCorrectionInterval(_property.as_string(), _check);
	}
	else
	{
		ret_value = Node::SetProperty(_property, _check);
	}

	return	ret_value;
}


bool    Gateway::GetProperty(uint32_t _type, JSONNode& _property)
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

/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Gateway::IsAttached(std::string const& _device_id)
{
	for(std::list<std::string>::iterator it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
	{
		if ((*it) == _device_id)
		{	
			return	true;
		}
	}

	return	false;
}

bool	Gateway::Attach(std::string const& _device_id)
{
	if (!IsAttached(_device_id))
	{
		device_id_list_.push_back(_device_id);
		TRACE_INFO("The device[" << _device_id << "] has been attahced.");
	}

	return	true;
}

bool	Gateway::Detach(std::string const& _device_id)
{
	for(std::list<std::string>::iterator it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
	{
		if ((*it) == _device_id)
		{	
			device_id_list_.erase(it);
			TRACE_INFO("The device[" << _device_id << "] has been detahced.");
			return	true;
		}
	}


	return	false;
}

bool	Gateway::Detach()
{
	device_id_list_.clear();
	return	true;
}

uint32_t	Gateway::GetDeviceCount()
{
	return	device_id_list_.size();
}

const std::string&	Gateway::GetDeviceAt(int index)
{
	static	std::string	null_id("");

	if (index >= 0)
	{
		for(std::list<std::string>::iterator it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
		{
			if (index == 0)
			{	
				return	*it;
			}

			index--;
		}
	}

	return	null_id;
}

bool		Gateway::GetDeviceList(std::list<std::string>& _device_id_list)
{
	_device_id_list = device_id_list_;

	return	true;
}

bool		Gateway::GetDeviceMap(std::map<std::string, Device*>& _map)
{
	for(std::list<std::string>::iterator it = device_id_list_.begin(); it != device_id_list_.end(); it++)
	{
		Device* device = manager_.GetDevice(*it);
		if (device != NULL)
		{
			_map[*it] = device;
		}
	}

	return	true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Utility

Gateway::operator JSONNode() 
{
	JSONNode	properties;

	Object::GetProperties(properties);

	return	properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Gateway::Preprocess()
{
	Node::Preprocess();


	for(std::list<std::string>::iterator it = device_id_list_.begin(); it != device_id_list_.end(); it++)
	{
		Device*	device = manager_.GetDevice(*it);
		if (device != NULL)
		{
			device->Start(DEVICE_START_INITIALIZATION_TIMEOUT);	
		}
		else
		{
			TRACE_ERROR("Device[" << (*it) << "] not found!");	
		}
	}
}

//add SY.KANG
void	Gateway::InfoProcess()
{
	Node::InfoProcess();
}
//

void	Gateway::Process()
{	
	Node::Process();
	
	//add SY.KANG
	InfoProcess();
	//
}

void	Gateway::Postprocess()
{
	for(std::list<std::string>::iterator it = device_id_list_.begin(); it != device_id_list_.end(); it++)
	{
		Device*	device = manager_.GetDevice(std::string(*it));
		if (device != NULL)
		{
			device->Stop();	
		}
	}

	Node::Postprocess();

}

Gateway*	Gateway::Create(ObjectManager& _manager, JSONNode const& _properties)
{
	Gateway*	gateway = NULL;

	try
	{
		std::string	type = JSONNodeGetType(_properties);

		if (type == GatewayGen::Type())
		{
			gateway = new GatewayGen(_manager, _properties);
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create gateway. Gateway type[" << type << "] is not supported!");
		}
	}
	catch(ObjectNotFound& e)
	{
		TRACE_ERROR2(NULL, "Failed to create gateway. Gateway type unknown!");
	}

	return	gateway;
}

bool	Gateway::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Node::GetPropertyFieldList(_field_list))
	{
	//	_field_list.push_back(TITLE_NAME_IP);
		_field_list.push_back(TITLE_NAME_CORRECTION_INTERVAL);	
	}

	return	true;
}


bool	Gateway::IsIncludeIn(Object *_object)
{
	return	dynamic_cast<Gateway*>(_object) != NULL;
}

////////////////////////////////////////////////////////////////////////////////
//	Define static members
////////////////////////////////////////////////////////////////////////////////
const char*	Gateway::Type()
{
	return	OBJECT_TYPE_GATEWAY;
}


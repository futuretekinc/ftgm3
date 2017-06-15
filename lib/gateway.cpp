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

Gateway::Gateway(ObjectManager& _manager, ValueType const& _type)
:	Node(_manager, _type)
{
}

Gateway::~Gateway()
{
}

#if 0
bool	Gateway::GetProperties(Properties& _properties, Properties::Fields const& _fields) 
{
	if (Node::GetProperties(_properties, _fields))
	{
#if 0
		if(_fields.child)
		{
			PropertiesList	device_properties_list;

			for(auto it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
			{
				Properties	device_properties;

				Device *device = manager_.GetDevice(*it);
				if (device != NULL)
				{
					device->GetProperties(device_properties, _fields);	
					device_properties_list.Append(device_properties);	
				}
				else
				{
					TRACE_INFO("Device[" << *it << "] not found!");	
				}
			}

			if (device_properties_list.size() != 0)
			{
				_properties.Append(Property(TITLE_NAME_DEVICE, device_properties_list));
			}
			else
			{
				TRACE_INFO("Device list is null!");	
			}
		}
#endif
		return	true;
	}

	return	false;
}

bool	Gateway::GetProperties(JSONNode& _properties, Properties::Fields const& _fields)
{
	return	Node::GetProperties(_properties, _fields);
}
#endif
bool	Gateway::SetProperty(Property const& _property, Properties::Fields const& _fields)
{
	bool	ret_value = true;

	if (_property.GetName() == TITLE_NAME_DEVICE)
	{
	}
	else
	{
		ret_value = Node::SetProperty(_property, _fields);
	}

	return	ret_value;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// Endpoint operation

bool	Gateway::IsAttached(ValueID const& _device_id)
{
	for(auto it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
	{
		if ((*it) == _device_id)
		{	
			return	true;
		}
	}

	return	false;
}

bool	Gateway::Attach(ValueID const& _device_id)
{
	if (!IsAttached(_device_id))
	{
		device_id_list_.push_back(_device_id);
		TRACE_INFO("The device[" << _device_id << "] has been attahced.");
	}

	return	true;
}

bool	Gateway::Detach(ValueID const& _device_id)
{
	for(auto it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
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

const ValueID&	Gateway::GetDeviceAt(int index)
{
	static	ValueID	null_id("");

	if (index >= 0)
	{
		for(auto it = device_id_list_.begin() ; it != device_id_list_.end() ; it++)
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

bool		Gateway::GetDeviceList(std::list<ValueID>& _device_id_list)
{
	_device_id_list = device_id_list_;

	return	true;
}

bool		Gateway::GetDeviceMap(std::map<std::string, Device*>& _map)
{
	for(auto it = device_id_list_.begin(); it != device_id_list_.end(); it++)
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
#if 0
	PropertiesList	properties_list;

	for(auto it = device_id_list_.begin(); it != device_id_list_.end() ; it++)
	{
		Properties	device_properties;
		Device*	device = manager_.GetDevice(std::string(*it));
		if (device != NULL)
		{
			device->GetProperties(device_properties);
			properties_list.Append(device_properties);
		}
	}

	properties.Append(Property(TITLE_NAME_DEVICE, properties_list));
#endif
	return	properties;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Ineternal operation
void	Gateway::Preprocess()
{
	Node::Preprocess();

	for(auto it = device_id_list_.begin(); it != device_id_list_.end(); it++)
	{
		Device*	device = manager_.GetDevice(*it);
		if (device != NULL)
		{
			device->Start();	
		}
		else
		{
			TRACE_ERROR("Device[" << (*it) << "] not found!");	
		}
	}
}

void	Gateway::Process()
{
	Node::Process();
}

void	Gateway::Postprocess()
{
	for(auto it = device_id_list_.begin(); it != device_id_list_.end(); it++)
	{
		Device*	device = manager_.GetDevice(std::string(*it));
		if (device != NULL)
		{
			device->Stop();	
		}
	}

	Node::Postprocess();

}

Gateway*	Gateway::Create(ObjectManager& _manager, Properties const& _properties)
{
	Gateway*	gateway = NULL;
	const Property *type_property = _properties.Get(TITLE_NAME_TYPE);

	if (type_property != NULL)
	{
		const ValueString*	type_value = dynamic_cast<const ValueString*>(type_property->GetValue());
		if (type_value != NULL)
		{
			Properties	properties(_properties);

			properties.Delete(TITLE_NAME_TYPE);

			if (std::string(*type_value) == std::string(GatewayGen::Type()))
			{
				gateway = new GatewayGen(_manager, properties);
			}
			else
			{
				TRACE_ERROR2(NULL, "Failed to create gateway. Gateway type[" << type_value->Get() << "] is not supported!");
			}
		}
		else
		{
			TRACE_ERROR2(NULL, "Failed to create gateway. Gateway type is invalid!");
		}
	}
	else
	{
		TRACE_ERROR2(NULL, "Failed to create gateway. Gateway type unknown!");
	}

	return	gateway;
}

const	ValueType&	Gateway::Type()
{
	static	ValueType	type_("gateway");

	return	type_;
}

bool		Gateway::IsValidType(std::string const& _type)
{
	if (_type == std::string(GatewayGen::Type()))
	{
		return	true;	
	}

	TRACE_ERROR2(NULL, "Gateway type[" << _type << "] invlaid");
	return	false;
}

bool	Gateway::GetPropertyFieldList(std::list<std::string>& _field_list)
{
	if (Node::GetPropertyFieldList(_field_list))
	{
		_field_list.push_back(TITLE_NAME_IP);
	}

	return	true;
}


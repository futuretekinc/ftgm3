#include <fstream>
#include <libjson/libjson.h>
#include "defined.h"
#include "trace.h"
#include "object_manager.h"
#include "device.h"
#include "endpoint.h"
#include "property.h"

ObjectManager::ObjectManager()
: ActiveObject()
{
	trace.SetClassName(GetClassName());
	name_ 	= "object_manager";
	enable_ = true;

	TRACE_INFO("ObjectManager : " << GetTraceName());
}

ObjectManager::~ObjectManager()
{
	for(auto it = device_map_.begin() ; it != device_map_.end() ; it++)
	{
		delete it->second;
	}
}

bool ObjectManager::Attach(DataManager* _data_manager)
{
	data_manager_ = _data_manager;

	
	return	true;
}

bool ObjectManager::Load(std::string const&  _file_name)
{
	std::fstream	fs(_file_name, std::fstream::in);
	if (fs)
	{
		fs.seekg (0, fs.end);
		int length = fs.tellg();
		fs.seekg (0, fs.beg);

		char * buffer = new char [length + 1];
		fs.read(buffer, length);
		buffer[length] = 0;
		fs.close();

		Load(buffer);

		delete buffer;
	}

	return	false;
}

bool ObjectManager::Load(const char *_buffer)
{
	if (!libjson::is_valid(_buffer))
	{
		std::cout << "Invalid json format" << std::endl;
		return	false;
	}

	JSONNode	json = libjson::parse(_buffer);

	return	Load(json);
}

bool	ObjectManager::Load(JSONNode const& _json)
{
	if (_json.name() == "devices")
	{
		if (_json.type() != JSON_ARRAY)
		{
			return	false;
		}

		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			Properties	properties;

			properties.Append(*it);

			Device *device = CreateDevice(properties);
			if (device == NULL)
			{
				TRACE_ERROR("Failed to create device!");
			}
		}
	}
	else if (_json.type() != JSON_NODE)
	{
		std::cout << "Invalid json format" << std::endl;
		return	false;
	}
	else
	{
		for(auto it = _json.begin(); it != _json.end() ; it++)
		{
			if (!Load(*it))
			{
				std::cout << "Invalid format" << std::endl;
				return	false;
			}
		}
	}

	return	true;
}

Device*		ObjectManager::CreateDevice(Properties const& _properties, bool from_db)
{
	Device*	device = Device::Create(*this, _properties);
	if (device != NULL)
	{
		if ((!from_db) && (data_manager_ != NULL))
		{
			data_manager_->AddDevice(device);
		}
	}

	return	device;
}

bool		ObjectManager::DestroyDevice(std::string const& _id)
{
	Device* device = GetDevice(_id);
	if (device == NULL)
	{
		TRACE_ERROR("Failed to delete device because the device[" << _id << "] coult not be found.");
		return	false;	
	}

	Detach(device);

	delete device;

	return	true;
}

uint32_t	ObjectManager::GetDeviceCount()
{
	return	device_map_.size();
}

uint32_t	ObjectManager::GetDeviceList(std::list<Device*>& _device_list)
{
	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		_device_list.push_back(it->second);	
	}

	return	_device_list.size();
}

uint32_t	ObjectManager::GetDeviceList(Device::Type _type, std::list<Device*>& _device_list)
{
	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		if (it->second->IsIncludedIn(_type))
		{
			_device_list.push_back(it->second);	
		}
	}

	return	_device_list.size();
}

Device*		ObjectManager::GetDevice(std::string const& _id)
{
	auto it = device_map_.find(_id);
	if (it != device_map_.end())
	{
		return	it->second;	
	}

	for(auto it = device_map_.begin(); it != device_map_.end() ; it++)
	{
		TRACE_ERROR("Device : " << it->first);
	}

	TRACE_ERROR("Device[" << _id << "] not found!");
	return	NULL;
}

Endpoint*	ObjectManager::CreateEndpoint(Properties const& _properties, bool from_db)
{
	Endpoint*	endpoint = Endpoint::Create(*this, _properties);		
	if (endpoint != NULL)
	{
		TRACE_INFO("From DB : " << from_db);
		if ((!from_db) && (data_manager_ != NULL))
		{
			data_manager_->AddEndpoint(endpoint);
		}
	}
	else
	{
		TRACE_ERROR("Failed to create endpoint");	
	}

	return	endpoint;
}

bool		ObjectManager::DestroyEndpoint(std::string const& _endpoint_id)
{
	Endpoint*	endpoint = GetEndpoint(_endpoint_id);
	if (endpoint != NULL)
	{
		Device*	device = GetDevice(endpoint->GetDeviceID());
		if (device != NULL)
		{
			device->Detach(_endpoint_id);	
		}
		
		auto it = endpoint_map_.find(endpoint->GetID());
		if (it != endpoint_map_.end())
		{
			endpoint_map_.erase(it);
		}

		delete endpoint;
		return	true;
	}

	return	false;
}

uint32_t	ObjectManager::GetEndpointCount()
{
	return	endpoint_map_.size();
}

uint32_t	ObjectManager::GetEndpointList(std::list<Endpoint*>& _endpoint_list)
{
	for(auto it = endpoint_map_.begin(); it != endpoint_map_.end() ; it++)
	{
		_endpoint_list.push_back(it->second);	
	}

	return	_endpoint_list.size();
}

Endpoint*		ObjectManager::GetEndpoint(std::string const& _id)
{
	auto it = endpoint_map_.find(_id);
	if (it != endpoint_map_.end())
	{
		return	it->second;	
	}

	return	NULL;
}

bool	ObjectManager::IDChanged(Device* _device, ValueID const& _old_id)
{
	auto it = device_map_.find(std::string(_old_id));
	if (it != device_map_.end())
	{
		device_map_.erase(it);
		device_map_[_device->GetID()] = _device;
		return	true;	
	}

	return	false;
}

bool	ObjectManager::IDChanged(Endpoint* _endpoint, ValueID const& _old_id)
{
	auto it = endpoint_map_.find(std::string(_old_id));
	if (it != endpoint_map_.end())
	{
		endpoint_map_.erase(it);
		endpoint_map_[_endpoint->GetID()] = _endpoint;
		return	true;	
	}

	return	false;
}

bool	ObjectManager::UpdateProperties(Object* _object)
{
	if (data_manager_ == NULL)
	{
		TRACE_ERROR("Failed to update properties becasuse data manager is not attached.");
		return	false;	
	}

	Device* device = dynamic_cast<Device*>(_object);
	if (device != NULL)
	{
		return	UpdateProperties(device);
	}
	else
	{
		Endpoint* endpoint= dynamic_cast<Endpoint*>(_object);
		if (endpoint != NULL)
		{
			return	UpdateProperties(endpoint);
		}
	}

	return	false;
}

bool	ObjectManager::UpdateProperties(Device* _device)
{
	if (data_manager_ == NULL)
	{
		TRACE_ERROR("Failed to update properties becasuse data manager is not attached.");
		return	false;	
	}

	Properties	properties;
	if (_device->GetUpdatedProperties(properties))
	{
		properties.Delete(OBJECT_FIELD_ID);

		if (!data_manager_->SetDeviceProperties(_device->GetID(), properties))
		{
			TRACE_ERROR("Failed to set device[" << _device->GetTraceName() << "] properties!");	
			return	false;
		}

		_device->ApplyChanges();
	}
	else
	{
		TRACE_ERROR("Failed to get device[" << _device->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::UpdateProperties(Endpoint* _endpoint)
{
	if (data_manager_ == NULL)
	{
		TRACE_ERROR("Failed to update properties becasuse data manager is not attached.");
		return	false;	
	}

	Properties	properties;
	if (_endpoint->GetUpdatedProperties(properties))
	{
		properties.Delete(OBJECT_FIELD_ID);

		if (!data_manager_->SetDeviceProperties(_endpoint->GetID(), properties))
		{
			TRACE_ERROR("Failed to set endpoint[" << _endpoint->GetTraceName() << "] properties!");	
			return	false;
		}

		_endpoint->ApplyChanges();
	}
	else
	{
		TRACE_ERROR("Failed to get endpoint[" << _endpoint->GetTraceName() << "] properties!");
		return	false;
	}

	return	true;
}

bool	ObjectManager::UpdateProperties(std::string const& _id)
{
	auto device_it = device_map_.find(_id);
	if (device_it != device_map_.end())
	{
		Device* device = device_it->second;
	
		return	UpdateProperties(device);
	}

	auto endpoint_it = endpoint_map_.find(_id);
	if (endpoint_it != endpoint_map_.end())
	{
		Endpoint* endpoint = endpoint_it->second;
	
		return	UpdateProperties(endpoint);
	}

	return	false;

}

bool	ObjectManager::AddData(std::string const& _endpoint_id, Date const& _date, Value const* _value)
{
	if (data_manager_ != NULL)
	{
		return	data_manager_->AddValue(_endpoint_id, _date, _value);
	}
	return	false;
}

void	ObjectManager::Preprocess()
{
	if (data_manager_ != NULL)
	{
		uint32_t count = data_manager_->GetDeviceCount();
		TRACE_INFO("Device Count : " << count);

		for(uint32_t i = 0 ; i < count ; i++)
		{
			std::list<Properties> properties_list;

			if (data_manager_->GetDeviceProperties(i, 1, properties_list))
			{
				for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
				{
					CreateDevice(*it, true);
				}
			}
		}

		count = data_manager_->GetEndpointCount();
		TRACE_INFO("Endpoint Count : " << count);

		for(uint32_t i = 0 ; i < count ; i++)
		{
			std::list<Properties> properties_list;

			if (data_manager_->GetEndpointProperties(i, 1, properties_list))
			{
				for(auto it = properties_list.begin() ; it != properties_list.end() ; it++)
				{
					const Property*	id_property = it->Get(OBJECT_FIELD_ID);
					if (id_property != NULL)
					{
						CreateEndpoint(*it, true);						
					}
				}
			}
		}
	}

}

void	ObjectManager::OnMessage(Message* _base_message)
{
}

void	ObjectManager::Process()
{
	ActiveObject::Process();
}

void	ObjectManager::Postprocess()
{
	ActiveObject::Postprocess();
}



bool	ObjectManager::Attach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it != device_map_.end())
	{
		TRACE_ERROR("The device[" << _device->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	device_map_[_device->GetID()] = _device;

	TRACE_INFO("The device[" << _device->GetTraceName() << "] has been attached."); 
	return	true;
}

bool	ObjectManager::Detach(Device* _device)
{
	auto it = device_map_.find(_device->GetID());
	if (it == device_map_.end())
	{
		TRACE_ERROR("The device[" << _device->GetTraceName() << "] not attached.");
		return	false;	
	}

	device_map_.erase(it);

	return	true;
}

bool	ObjectManager::Attach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it != endpoint_map_.end())
	{
		TRACE_ERROR("The endpoint[" << _endpoint->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	endpoint_map_[_endpoint->GetID()] = _endpoint;
	_endpoint->parent_ = this;

	std::string	device_id = _endpoint->GetDeviceID();
	if (!device_id.empty())
	{
		Device* device = GetDevice(_endpoint->GetDeviceID());
		if (device != NULL)
		{
			device->Attach(_endpoint->GetID());	
		}
	}
	TRACE_INFO("The endpoint[" << _endpoint->GetTraceName() << "] has been attached."); 

	return	true;
}

bool	ObjectManager::Detach(Endpoint* _endpoint)
{
	auto it = endpoint_map_.find(_endpoint->GetID());
	if (it == endpoint_map_.end())
	{
		TRACE_ERROR("The endpoint[" << _endpoint->GetTraceName() << "] not attached.");
		return	false;	
	}

	endpoint_map_.erase(it);
	_endpoint->parent_ = this;

	return	true;
}

bool	ObjectManager::Attach(RemoteMessageServer* _rms)
{
	auto it = rms_map_.find(_rms->GetID());
	if (it != rms_map_.end())
	{
		TRACE_ERROR("The rms[" << _rms->GetTraceName() << "] has been already attached.");
		return	false;	
	}

	rms_map_[_rms->GetID()] = _rms;
	_rms->SetObjectManager(this);

	TRACE_INFO("The rms[" << _rms->GetTraceName() << "] has been attached."); 

	return	true;
}

bool	ObjectManager::Detach(RemoteMessageServer* _rms)
{
	auto it = rms_map_.find(_rms->GetID());
	if (it == rms_map_.end())
	{
		TRACE_ERROR("The rms[" << _rms->GetTraceName() << "] not attached.");
		return	false;	
	}

	rms_map_.erase(it);
	_rms->SetObjectManager(NULL);

	return	true;
}
